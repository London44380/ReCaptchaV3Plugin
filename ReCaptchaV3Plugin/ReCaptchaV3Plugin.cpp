#include "ReCaptchaV3Plugin.h"
#include <curl/curl.h>
#include <string>
#include <cstring>

static const char* RECAPTCHA_VERIFY_URL =
    "https://www.google.com/recaptcha/api/siteverify";

struct ReCaptchaConfig {
    std::string secretKey;
    double minScore = 0.5;
    bool initialized = false;
};

static ReCaptchaConfig g_cfg;
static bool g_curlInited = false;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    std::string* s = static_cast<std::string*>(userp);
    s->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

// Parsing JSON très simple (sans lib externe) pour success / score / action.
static bool parse_recaptcha_response(
    const std::string& json,
    bool& success,
    double& score,
    std::string& action
) {
    success = false;
    score = 0.0;
    action.clear();

    auto findBool = [&](const char* key) -> bool {
        std::string k = "\"";
        k += key;
        k += "\":";
        auto pos = json.find(k);
        if (pos == std::string::npos) return false;
        pos += k.size();
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        if (json.compare(pos, 4, "true") == 0) {
            success = true;
            return true;
        }
        if (json.compare(pos, 5, "false") == 0) {
            success = false;
            return true;
        }
        return false;
    };

    auto findNumber = [&](const char* key, double& out) -> bool {
        std::string k = "\"";
        k += key;
        k += "\":";
        auto pos = json.find(k);
        if (pos == std::string::npos) return false;
        pos += k.size();
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        std::string num;
        while (pos < json.size() &&
               ((json[pos] >= '0' && json[pos] <= '9') || json[pos] == '.')) {
            num.push_back(json[pos++]);
        }
        if (num.empty()) return false;
        out = std::stod(num);
        return true;
    };

    auto findString = [&](const char* key, std::string& out) -> bool {
        std::string k = "\"";
        k += key;
        k += "\":";
        auto pos = json.find(k);
        if (pos == std::string::npos) return false;
        pos += k.size();
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        if (pos >= json.size() || json[pos] != '\"') return false;
        pos++;
        std::string val;
        while (pos < json.size() && json[pos] != '\"') {
            val.push_back(json[pos++]);
        }
        out = val;
        return true;
    };

    bool okSucc = findBool("success");
    bool okScore = findNumber("score", score);
    std::string act;
    bool okAction = findString("action", act);

    if (okAction) action = act;
    return okSucc && okScore;
}

extern "C" RECAPTCHA_API int ReCaptcha_Init(const char* secretKey, double minScore) {
    if (!secretKey || std::strlen(secretKey) == 0) {
        return RECAPTCHA_ERR_PARSE;
    }

    g_cfg.secretKey = secretKey;
    g_cfg.minScore = (minScore > 0.0 ? minScore : 0.5);

    CURLcode cc = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (cc != CURLE_OK) {
        return RECAPTCHA_ERR_CURL;
    }
    g_curlInited = true;
    g_cfg.initialized = true;
    return RECAPTCHA_OK;
}

extern "C" RECAPTCHA_API int ReCaptcha_Verify(
    const char* token,
    const char* action,
    const char* clientIp,
    double* outScore
) {
    if (!g_cfg.initialized) return RECAPTCHA_ERR_NOT_INIT;
    if (!token || std::strlen(token) == 0) return RECAPTCHA_ERR_PARSE;

    CURL* curl = curl_easy_init();
    if (!curl) return RECAPTCHA_ERR_CURL;

    std::string postData = "secret=" + g_cfg.secretKey +
                           "&response=" + std::string(token);
    if (clientIp && std::strlen(clientIp) > 0) {
        postData += "&remoteip=";
        postData += clientIp;
    }

    curl_easy_setopt(curl, CURLOPT_URL, RECAPTCHA_VERIFY_URL);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

    // timeouts raisonnables
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    }
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return RECAPTCHA_ERR_CURL;
    }
    if (httpCode != 200) {
        return RECAPTCHA_ERR_HTTP;
    }

    bool success = false;
    double score = 0.0;
    std::string respAction;
    if (!parse_recaptcha_response(response, success, score, respAction)) {
        return RECAPTCHA_ERR_PARSE;
    }

    if (action && std::strlen(action) > 0) {
        if (respAction != action) {
            return RECAPTCHA_ERR_PARSE;
        }
    }

    if (outScore) {
        *outScore = score;
    }

    if (!success || score < g_cfg.minScore) {
        return RECAPTCHA_ERR_SCORE_LOW;
    }

    return RECAPTCHA_OK;
}

extern "C" RECAPTCHA_API void ReCaptcha_Cleanup(void) {
    g_cfg.initialized = false;
    g_cfg.secretKey.clear();
    if (g_curlInited) {
        curl_global_cleanup();
        g_curlInited = false;
    }
}
