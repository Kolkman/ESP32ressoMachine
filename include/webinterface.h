#ifndef ESPRressoMach_WEB_H
#define ESPRressoMach_WEB_H
#include <WiFi.h>
#include <AsyncTCP.h>
#include "EspressoWebServer.h"
#include "ESPressoMachine.h"

class WebInterface
{
public:
    WebInterface(ESPressoMachine *, const char *username = "", const char *password = "");
    ~WebInterface();
    void setupWebSrv(ESPressoMachine *);
    void setConfigPortalPages();
    void unsetConfigPortalPages();
    void eventLoop();
    EspressoWebServer *server;
    bool _waitingForClientAction = false;

protected:
    ESPressoMachine *myMachine;

private:
    AsyncEventSource *events;
    String _username;
    String _password;
    bool _authRequired = false;
    // HTTPUpdateServer *httpUpdater;
    void handleRoot(AsyncWebServerRequest *);
    void handleRestart(AsyncWebServerRequest *);
    void handleEventClient(AsyncEventSourceClient *);
    void handleCaptivePortal(AsyncWebServerRequest *);
    void handleScan(AsyncWebServerRequest *);
    void handleConfigConfig(AsyncWebServerRequest *);
    bool captivePortal(AsyncWebServerRequest *);
    void handleNetworkSetup(AsyncWebServerRequest *);



    unsigned long remainingPortaltime();
    bool isIp(const String &);
    unsigned long _configPortalInterfaceStart = 0;

};



#endif