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
    memset(password, '\0', CREDENTIAL_LENGTH);
    strncpy(password, p, std::max(strlen(p), (size_t) (CREDENTIAL_LENGTH - 1)));
}
void EspressoWebServer::setUsername(const char *u = "")
{
    memset(username, '\0', CREDENTIAL_LENGTH);
    strncpy(username, u, std::max(strlen(u), (size_t) (CREDENTIAL_LENGTH - 1)));
}

void EspressoWebServer::authenticate(AsyncWebServerRequest *request)
{
    Serial.println("authenticate for:" + request->url());
    if (authRequired)
    {

        if (!request->authenticate(this->getUsername(), this->getPassword()))
        {

            return request->requestAuthentication();
        }
    }
}
