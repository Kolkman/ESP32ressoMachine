#ifndef ESPRressoMach_WEB_H
#define ESPRressoMach_WEB_H

#include "webinterface.h"
#include <WebServer.h>
#include <HTTPUpdateServer.h>






class WebInterface
{
public:

    WebInterface();
    ~ WebInterface();
    void setupWebSrv(ESPressoMachine *);
    void loopWebSrv();
private:
    ESPressoMachine *myMachine;
    WebServer *server;
    HTTPUpdateServer *httpUpdater;

    void handleNotFound();
    void handleRoot();
    void handleConfig();
    void handleTuningStats();
    void handleSetConfig();
    void handleSetTuning();
    void handleLoadConfig();
    void handleSaveConfig();
    void handleResetConfig();
    void handleToggleHeater();
    void handleHeaterSwitch(boolean);
    void handleHeaterOn();
    void handleHeaterOff();
    void handlePidOn();
    void handlePidOff();
    void handleTuningMode();
    void handleReset();
    void handleApiStatus();
    void handleApiFirmware();   
    void handleCSS();
};

#endif