#ifndef RECAPTCHAV3PLUGIN_H
#define RECAPTCHAV3PLUGIN_H

#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef RECAPTCHA_EXPORTS
    #define RECAPTCHA_API __declspec(dllexport)
  #else
    #define RECAPTCHA_API __declspec(dllimport)
  #endif
#else
  #if __GNUC__ >= 4
    #define RECAPTCHA_API __attribute__ ((visibility ("default")))
  #else
    #define RECAPTCHA_API
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

    // Codes retour
    enum ReCaptchaResult {
        RECAPTCHA_OK           = 0,
        RECAPTCHA_ERR_NOT_INIT = -1,
        RECAPTCHA_ERR_CURL     = -2,
        RECAPTCHA_ERR_HTTP     = -3,
        RECAPTCHA_ERR_PARSE    = -4,
        RECAPTCHA_ERR_SCORE_LOW= -5
    };

    // Init avec clé secrète côté serveur + score min accepté (ex: 0.5)
    RECAPTCHA_API int ReCaptcha_Init(const char* secretKey, double minScore);

    // Vérifie un token v3 côté serveur.
    //  token     : token reçu du client
    //  action    : action attendue ("login", "submit", ...), peut être NULL
    //  clientIp  : IP optionnelle (remoteip), peut être NULL
    //  outScore  : score retourné par Google (optionnel, peut être NULL)
    // Retour : ReCaptchaResult
    RECAPTCHA_API int ReCaptcha_Verify(
        const char* token,
        const char* action,
        const char* clientIp,
        double* outScore
    );

    // Libère les ressources globales
    RECAPTCHA_API void ReCaptcha_Cleanup(void);

#ifdef __cplusplus
}
#endif

#endif // RECAPTCHAV3PLUGIN_H
