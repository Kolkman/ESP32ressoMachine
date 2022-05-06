// Code fragments from
// https://github.com/lbernstone/asyncUpdate/blob/master/AsyncUpdate.ino

#include "webInterfaceOTAUpdate.h"
#include <Update.h>
#include <ESPAsyncWebServer.h>
#include "pages/update.html.h"

webInterfaceOTAUpdate webOTAUpdate;

void webInterfaceOTAUpdate::begin(EspressoWebServer *server)
{
    _server = server;

   
    _server->on("/update/", HTTP_GET, [&](AsyncWebServerRequest *request){
          request->redirect("/update.html");
    });

    _server->on("/update", HTTP_GET, [&](AsyncWebServerRequest *request){
          request->redirect("/update.html");
    });

    _server->on("/update.html", HTTP_GET, [&](AsyncWebServerRequest *request)
                {
        _server->authenticate(request);
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", update_html, update_html_len);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response); });

    server->on(
        "/doUpdate", HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        std::bind(&webInterfaceOTAUpdate::handleDoUpdate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

    Update.onProgress(std::bind(&webInterfaceOTAUpdate::printProgress, this, std::placeholders::_1, std::placeholders::_2));
}

void webInterfaceOTAUpdate::handleDoUpdate(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
    _server->authenticate(request);

    if (!index)
    {
        Serial.println("Update");

        if (!request->hasParam("MD5", true))
        {
            return request->send(400, "text/plain", "MD5 parameter missing");
        }

        if (!Update.setMD5(request->getParam("MD5", true)->value().c_str()))
        {
            return request->send(400, "text/plain", "MD5 parameter invalid");
        }

        content_len = request->contentLength();
        // if filename includes spiffs, update the spiffs partition
        int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
        // #ifdef ESP8266 // In case of future porting
        //        Update.runAsync(true);
        //        if (!Update.begin(content_len, cmd))
        //        {
        // #else
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd))
        {
            // #endif
            Update.printError(Serial);
            return request->send(400, "text/plain", "OTA could not begin");
        }
    }

    if (Update.write(data, len) != len)
    {
        Update.printError(Serial);
    }

    if (final)
    {
        AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
        response->addHeader("Refresh", "20");
        response->addHeader("Location", "/");
        request->send(response);
        if (!Update.end(true))
        {
            Update.printError(Serial);
        }
        else
        {
            Serial.println("Update complete");
            Serial.flush();
            ESP.restart();
        }
    }
}

void webInterfaceOTAUpdate::printProgress(size_t prg, size_t sz)
{
    Serial.printf("Progress: %d%%\n", (prg * 100) / content_len);
}