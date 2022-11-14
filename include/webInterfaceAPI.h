#ifndef ESPRressoMach_WEB_API_H
#define ESPRressoMach_WEB_API_H
#include <Arduino.h>
#include "EspressoWebServer.h"
#include "ESPressoMachine.h"

class webInterfaceAPI
{
public:
    void begin(EspressoWebServer *server, ESPressoMachine *);
    void requireAuthorization(bool);
    webInterfaceAPI();

private:
    EspressoWebServer *server;
    ESPressoMachine *myMachine;
    bool mustAuthenticate;
    size_t content_len;
    void handleStatus(AsyncWebServerRequest *);
    void handleFirmware(AsyncWebServerRequest *);
    void handleSet(AsyncWebServerRequest *);
    void handleGet(AsyncWebServerRequest *);
    void handleStats(AsyncWebServerRequest *);
    void handleConfigFile(AsyncWebServerRequest *);
    void handleNetwork(AsyncWebServerRequest *);
    void handleIsAuthenticated(AsyncWebServerRequest *);
};

extern webInterfaceAPI webAPI;

#endif