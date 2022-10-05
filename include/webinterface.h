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
    bool waitingForClientAction = false;

protected:
    ESPressoMachine *myMachine;

private:
    AsyncEventSource *events;
    String _username;
    String _password;
    bool _authRequired = false;
    // HTTPUpdateServer *httpUpdater;
    void handleNotFound(AsyncWebServerRequest *);
    void handleRoot(AsyncWebServerRequest *);
    void handleReset(AsyncWebServerRequest *);
    void handleEventClient(AsyncEventSourceClient *);
    void handleFile(AsyncWebServerRequest *, const char *, const unsigned char *, const size_t);
    void handleCaptivePortal(AsyncWebServerRequest *);
    void handleScan(AsyncWebServerRequest *);
    void handleConfigConfig(AsyncWebServerRequest *);
};

const char htmlHeader[] = "<!DOCTYPE HTML><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\"/><link rel=\"stylesheet\" type=\"text/css\" href=\"button.css\" media=\"all\" /><link rel=\"stylesheet\" type=\"text/css\" href=\"ESPresso.css\" media=\"all\"/><link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\"></head><body>";
const char htmlFooter[] = "</body></html>";

#endif