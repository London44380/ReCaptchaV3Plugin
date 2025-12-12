#include <iostream>
#include "ReCaptchaV3Plugin.h"

int main() {
    // À mettre côté serveur uniquement (jamais exposer la secret au client)
    int rc = ReCaptcha_Init("TON_SECRET_KEY", 0.5);
    if (rc != RECAPTCHA_OK) {
        std::cerr << "Init error: " << rc << "\n";
        return 1;
    }

    // Token normalement reçu depuis le client (form / XHR)
    const char* token = "TOKEN_RECU_DU_CLIENT";
    double score = 0.0;

    rc = ReCaptcha_Verify(token, "login", "1.2.3.4", &score);
    if (rc == RECAPTCHA_OK) {
        std::cout << "ReCaptcha OK, score=" << score << "\n";
    } else {
        std::cout << "ReCaptcha FAIL, code=" << rc << " score=" << score << "\n";
    }

    ReCaptcha_Cleanup();
    return 0;
}
