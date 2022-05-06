#ifndef ESPRressoMach_WEB_UPD_H
#define ESPRressoMach_WEB_UPD_H
#include <Arduino.h>
#include "EspressoWebServer.h"

class webInterfaceOTAUpdate
{
public:
    void begin(EspressoWebServer *server);
    void handleDoUpdate(AsyncWebServerRequest *, const String &, size_t, uint8_t *, size_t, bool);
    void printProgress(size_t, size_t);
    void restart();

private:
    EspressoWebServer *_server;
    size_t content_len;

};


extern webInterfaceOTAUpdate webOTAUpdate;


#endif