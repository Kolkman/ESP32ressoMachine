
#ifndef DEF_MYwebServer_H
#define DEF_MYwebServer_H
#include "ESPressoMachineDefaults.h"
#include <ESPAsyncWebServer.h>

const char htmlHeader[] = "<!DOCTYPE HTML><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\"/><link rel=\"stylesheet\" type=\"text/css\" href=\"button.css\" media=\"all\" /><link rel=\"stylesheet\" type=\"text/css\" href=\"ESPresso.css\" media=\"all\"/><link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\"></head><body>";
const char htmlFooter[] = "</body></html>";
#define CREDENTIAL_LENGTH 64


#ifndef COOKIENAME
#define COOKIENAME "ESP32RESSO_ID"
#endif

#ifndef COOKIEMAXAGE
#define COOKIEMAXAGE 60
#endif

class EspressoWebServer : public AsyncWebServer
{
public:
    EspressoWebServer(uint16_t, const char *, const char *);
    bool authRequired;

    void setPassword(const char *);
    void setUsername(const char *);
    void handleLogin(AsyncWebServerRequest *);
    void handleLogout(AsyncWebServerRequest *);
    void handleNotFound(AsyncWebServerRequest *);
    void handleFile(AsyncWebServerRequest *, const char *, const unsigned char *, const size_t);
    bool is_authenticated(AsyncWebServerRequest *);
    void authenticate(AsyncWebServerRequest *);
   
    void InitPages();
    //    void  authenticate(AsyncWebServerRequest *);
    char *getUsername();
    char *getPassword();

private:
    char username[CREDENTIAL_LENGTH + 1];
    char password[CREDENTIAL_LENGTH + 1];
    String sha1(String);
};

#endif