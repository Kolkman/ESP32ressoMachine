// Code fragments from
// https://github.com/lbernstone/asyncUpdate/blob/master/AsyncUpdate.ino

#include "webInterfaceOTAUpdate.h"
#include "pages/update.html.h"
#include <ESPAsyncWebServer.h>
#include <Update.h>

webInterfaceOTAUpdate webOTAUpdate;

webInterfaceOTAUpdate::webInterfaceOTAUpdate() {
  _server = nullptr;
  content_len = 0;
  UpdateError = true;
}

void webInterfaceOTAUpdate::begin(EspressoWebServer *server) {
  _server = server;

  _server->on("/update/", HTTP_GET, [&](AsyncWebServerRequest *request) {
    request->redirect("/update.html");
  });

  _server->on("/update", HTTP_GET, [&](AsyncWebServerRequest *request) {
    request->redirect("/update.html");
  });

  _server->on("/update.html", HTTP_GET, [&](AsyncWebServerRequest *request) {
    _server->authenticate(request);
    AsyncWebServerResponse *response =
        request->beginResponse(200, "text/html", update_html, update_html_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  _server->on(
      "/doUpdate", HTTP_POST, [](AsyncWebServerRequest *request) {},
      std::bind(&webInterfaceOTAUpdate::handleDoUpdate, this,
                std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6));

#ifdef TESTUPDATE
#warning "TESTUPDATE is defined, it is code used for testing purposes, undefine TESTUPDATE if you are not using the functionality"
  _server->on(
      "/testUpdate", HTTP_POST, [](AsyncWebServerRequest *request) {},
      std::bind(&webInterfaceOTAUpdate::testDoUpdate, this,
                std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3, std::placeholders::_4,
                std::placeholders::_5, std::placeholders::_6));
#endif

  Update.onProgress(std::bind(&webInterfaceOTAUpdate::printProgress, this,
                              std::placeholders::_1, std::placeholders::_2));

  UpdateError = false;
}

#ifdef TESTUPDATE
void webInterfaceOTAUpdate::testDoUpdate(AsyncWebServerRequest *request,
                                         const String &filename, size_t index,
                                         uint8_t *data, size_t len,
                                         bool final) {

  if (!index) {
    Serial.println("Update");

    content_len = request->contentLength();
  }

  if (final) {
    Serial.println("FINAL");
  }
}

#endif

void webInterfaceOTAUpdate::handleDoUpdate(AsyncWebServerRequest *request,
                                           const String &filename, size_t index,
                                           uint8_t *data, size_t len,
                                           bool final) {
  //_server->authenticate(request);

  if (!index) {
    Serial.println("Update");
    UpdateError = false;
    // Serial.println("Free Space: "+ESP.getFreeSketchSpace());

    /* MD5 is left for later - don't want to add JS bloat
    if (!request->hasParam("MD5", true))
    {
        Serial.println("MD5 Parameter missing");
        UpdateError = true;
        return request->send(400, "text/plain", "MD5 parameter missing");
    }
    if (!Update.setMD5(request->getParam("MD5", true)->value().c_str()))
    {
        UpdateError = true;
        Serial.println("MD5 Parameter invalid");
        return request->send(400, "text/plain", "MD5 parameter invalid");
    }
    */

    // Serial.print("Contentlen: ");
    content_len = request->contentLength();
    // Serial.println(content_len);

    // if filename includes spiffs, update the spiffs partition
    int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;

    if (!Update.begin(content_len, cmd)) // or UPDATE_SIZE_UNKNOWN
    {
      Serial.print("begin error: ");
      Update.printError(Serial);
      UpdateError = true;
      return request->send(400, "text/plain", "OTA could not begin");
    }
  }

  if (!Update.hasError() &&
      !UpdateError) // UpdateError is needed because it looks
                    // when one writes without .begin having been
                    // succesful an "No Error" is reported but the
                    // written length doesn't equal the offered length.
  {
    if (Update.write(data, len) != len) {
      Serial.print("Write error: ");
      Update.printError(Serial);
      return request->send(400, "text/plain", "OTA could not write");
    }

    if (final) {
      Serial.println("FINAL");
      AsyncWebServerResponse *response = request->beginResponse(
          302, "text/plain", "Please wait while the device reboots");
      response->addHeader("Refresh", "20");
      response->addHeader("Location", "/");
      request->send(response);
      if (!Update.end(true)) {
        Update.printError(Serial);
      } else {
        Serial.println("Update complete");
        Serial.flush();
        ESP.restart();
      }
    }
  }
}

void webInterfaceOTAUpdate::printProgress(size_t prg, size_t sz) {
  Serial.printf("Progress: %d%%\n", (prg * 100) / content_len);
}