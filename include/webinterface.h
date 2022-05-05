#ifndef ESPRressoMach_WEB_H
#define ESPRressoMach_WEB_H

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ESPressoMachine.h"

class WebInterface
{
public:
    WebInterface();
    ~WebInterface();
    void setupWebSrv(ESPressoMachine *);
    void eventLoop(String);

private:
    ESPressoMachine *myMachine;
    AsyncWebServer *server;
    AsyncEventSource *events;
    // HTTPUpdateServer *httpUpdater;

    void handleNotFound(AsyncWebServerRequest *);
    void handleRoot(AsyncWebServerRequest *);
    void handleConfig(AsyncWebServerRequest *);
    void handleTuningStats(AsyncWebServerRequest *);
    void handleSetConfig(AsyncWebServerRequest *);
    void handleSetTuning(AsyncWebServerRequest *);
    void handleLoadConfig(AsyncWebServerRequest *);
    void handleSaveConfig(AsyncWebServerRequest *);
    void handleResetConfig(AsyncWebServerRequest *);
    void handleToggleHeater(AsyncWebServerRequest *);
    void handleHeaterSwitch(AsyncWebServerRequest *, boolean);
    void handleHeaterOn(AsyncWebServerRequest *);
    void handleHeaterOff(AsyncWebServerRequest *);
    void handlePidOn(AsyncWebServerRequest *);
    void handlePidOff(AsyncWebServerRequest *);
    void handleTuningMode(AsyncWebServerRequest *);
    void handleReset(AsyncWebServerRequest *);
    void handleApiStatus(AsyncWebServerRequest *);
    void handleApiFirmware(AsyncWebServerRequest *);
    void handleApiSet(AsyncWebServerRequest *);
    void handleEventClient(AsyncEventSourceClient *);
    void handleFile(AsyncWebServerRequest *, const char *, const unsigned char *, const size_t);
};


const char htmlHeader[]="<!DOCTYPE HTML><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\"/><link rel=\"stylesheet\" type=\"text/css\" href=\"button.css\" media=\"all\" /><link rel=\"stylesheet\" type=\"text/css\" href=\"ESPresso.css\" media=\"all\"/><link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\"></head><body>";
const char htmlFooter[]="</body></html>";


#endif