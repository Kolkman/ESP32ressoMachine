//
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// Web Server with Options and Plotter
//

//#define ELEGANT_OTA

#include "ESPressoMachine.h"
#include <PID_v1.h>

#include "webinterface.h"
#include "config.h"
#include <Arduino.h>

#include "webInterfaceAPI.h"

#ifdef ELEGANT_OTA
#include <AsyncElegantOTA.h>
#else
#include "webInterfaceOTAUpdate.h"
#endif

// Various WebSources:
#include "pages/test.html.h"
#include "pages/configuration.html.h"
#include "pages/button.css.h"
#include "pages/ESPresso.css.h"
#include "pages/drawtimeseries.js.h"
#include "pages/firmware.js.h"
#include "pages/gauge.min.js.h"
#include "pages/EspressoMachine.svg.h"
#include "pages/index.html.h"

#define ONCOLOR "CD212A"
#define OFFCOLOR "DCDCDC"

WebInterface::WebInterface()
{
  Serial.println("Webinterfce Constructor");

  _username = "";
  _password = "";
  server = nullptr;
  myMachine = nullptr;
  events = nullptr;
  //  httpUpdater = new HTTPUpdateServer;
}

WebInterface::WebInterface(ESPressoMachine * machine)
{
  Serial.println("Webinterfce Constructor");

  _username = "";
  _password = "";
  server = nullptr;
  myMachine = machine;
  events = nullptr;
  //  httpUpdater = new HTTPUpdateServer;
}

WebInterface::~WebInterface()
{
  Serial.println("Webinterfce Destructor");
}

void WebInterface::handleNotFound(AsyncWebServerRequest *request)
{
  String message = htmlHeader;
  message += "<H1>Error 400 <br/> File Not Found</H1>\n\n";
  message += "<div id=\"notFoundInfo\"><div id=\"notFoundURI\">URI: <span id=\"notFoundURL\">";
  message += request->url();
  message += "</span></div>\n<div id=\"notFoundMethod\">Method: ";
  message += (request->method() == HTTP_GET) ? "GET" : "POST";
  message += "</div>\n<div id=\"notFoundArguments\">Arguments: ";
  message += request->args();
  message += "</div>\n";

  for (uint8_t i = 0; i < request->args(); i++)
  {
    message += "<div class=\"notFoundArgument\"><span class=\"notFoundargName\">" + request->argName(i) + "</span>:<span class=\"notFoundarg\"> " + request->arg(i) + "</span></div>\n";
  }
  message += "</div>";
  message += htmlFooter;
  request->send(404, "text/html", message);
}

void WebInterface::handleRoot(AsyncWebServerRequest *request)
{
  request->redirect("/index.html");
}








void WebInterface::handleFile(AsyncWebServerRequest *request, const char *mimetype, const unsigned char *compressedData, const size_t compressedDataLen)
{
  AsyncWebServerResponse *response = request->beginResponse_P(200, mimetype, compressedData, compressedDataLen);
  response->addHeader("Server", "ESP Async Web Server");
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}


void WebInterface::handleReset(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Reseting Device ! </h1>";
  request->send(200, "text/html", message);
  delay(1000);
  ESP.restart();
}

void WebInterface::setupWebSrv(ESPressoMachine *machine, const char *username, const char *password)
{
  myMachine = machine;

  Serial.println("Setting up  Webserver");
  server = new EspressoWebServer(80, username, password);
  if (server == nullptr)
    Serial.println("SERIAL NULLPTR");

  events = new AsyncEventSource("/events");
  if (events == nullptr)
    Serial.println("Events NULLPTR");

  // httpUpdater->setup(server);
  // Serial.print("Updater running !");

  server->on("/", HTTP_GET, std::bind(&WebInterface::handleRoot, this, std::placeholders::_1));
  server->onNotFound(std::bind(&WebInterface::handleNotFound, this, std::placeholders::_1));
  server->on("/reset", HTTP_GET, std::bind(&WebInterface::handleReset, this, std::placeholders::_1));
  webAPI.begin(server, myMachine);

#ifdef ELEGANT_OTA
  AsyncElegantOTA.begin(server);
#else
  webOTAUpdate.begin(server);
#endif
  // These are definitions generated by make - all to serve
  // static webpages

  server->on("/configure", HTTP_GET, [&](AsyncWebServerRequest *request)
             { request->redirect("/configuration.html"); });
  server->on("/config", HTTP_GET, [&](AsyncWebServerRequest *request)
             { request->redirect("/configuration.html"); });
  DEF_HANDLE_index_html;
  DEF_HANDLE_button_css;
  DEF_HANDLE_ESPresso_css;
  DEF_HANDLE_gauge_min_js;
  DEF_HANDLE_EspressoMachine_svg;
  DEF_HANDLE_drawtimeseries_js;
  DEF_HANDLE_firmware_js;
  DEF_HANDLE_configuration_html;
  DEF_HANDLE_test_html;

  // Handle Web Server Events
  events->onConnect(std::bind(&WebInterface::handleEventClient, this, std::placeholders::_1));
  server->addHandler(events);

  server->begin();
  Serial.println("HTTP server started");
}

void WebInterface::handleEventClient(AsyncEventSourceClient *client)
{

  if (client->lastId())
  {
    Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
  }
  // send event with message "hello!", id current millis
  // and set reconnect delay to 1 second
  client->send("hello!", NULL, millis(), myMachine->myConfig->pidInt);
}

void WebInterface::eventLoop()
{
  events->send(myMachine->machineStatus.c_str(), "status", millis());
}