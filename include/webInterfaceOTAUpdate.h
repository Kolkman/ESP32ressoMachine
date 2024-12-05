#ifndef ESPRressoMach_WEB_UPD_H
#define ESPRressoMach_WEB_UPD_H
#include "EspressoWebServer.h"
#include <Arduino.h>

class webInterfaceOTAUpdate {
public:
  webInterfaceOTAUpdate();
  void begin(EspressoWebServer *server);
  void handleDoUpdate(AsyncWebServerRequest *, const String &, size_t,
                      uint8_t *, size_t, bool);
#ifdef TESTUPDATE
  void testDoUpdate(AsyncWebServerRequest *, const String &, size_t, uint8_t *,
                    size_t, bool);
#endif
  void printProgress(size_t, size_t);
  void restart();

private:
  EspressoWebServer *_server;
  size_t content_len;
  bool UpdateError;
};

extern webInterfaceOTAUpdate webOTAUpdate;

#endif