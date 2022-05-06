#include "EspressoWebServer.h"
#include <ESPAsyncWebServer.h>

EspressoWebServer::EspressoWebServer(uint16_t port, const char *username, const char *password) : AsyncWebServer(port)
{
    setUsername(username);
    setPassword(password);
    if (strlen(username) > 0)
    {
        authRequired = true;
    }
    else
    {
        authRequired = false;
    }

}

char *EspressoWebServer::getUsername()
{
    return username;
}

char *EspressoWebServer::getPassword()
{
    return password;
}

void EspressoWebServer::setPassword(const char *p = "")
{
    strncpy(password, p, CREDENTIAL_LENGTH);
    password[CREDENTIAL_LENGTH + 1] = '\0';
}
void EspressoWebServer::setUsername(const char *u = "")
{
    strncpy(username, u, CREDENTIAL_LENGTH);
    username[CREDENTIAL_LENGTH + 1] = '\0';
}

void EspressoWebServer::authenticate(AsyncWebServerRequest *request)
{
    Serial.println("authenticate for:" + request->url());
    if (authRequired){
    
        if (!request->authenticate(this->getUsername(), this->getPassword()))
        {

            return request->requestAuthentication();
        }
    }
}
