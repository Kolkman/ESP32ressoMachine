#ifndef ESPRressoMach_WEB_UPD_H
#define ESPRressoMach_WEB_UPD_H
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class webInterfaceOTAUpdate
{
public:
    void begin(AsyncWebServer *server, const char *username = "", const char *password = "");
    void handleDoUpdate(AsyncWebServerRequest *, const String &, size_t, uint8_t *, size_t, bool);
    void printProgress(size_t, size_t);
    void restart();

private:
    AsyncWebServer *_server;
    size_t content_len;
    String _username = "";
    String _password = "";
    bool _authRequired = false;
};


extern webInterfaceOTAUpdate webOTAUpdate;


#endif