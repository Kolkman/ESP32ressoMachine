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

void WebInterface::handleTuningStats(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"5\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT Configuration</title></head><h1>EspressIoT</h1>\n";
  message += "<h1> PID TUNING STATS </h1>";
  message += "Total power-on-cycles: " + String(myMachine->myTuner->getTuneCount()) + "<br/>\n";
  message += "Time elapsed: " + String(myMachine->myTuner->timeElapsed()) + " ms<br/>\n";
  message += "Average Period: " + String(myMachine->myTuner->averagePeriod()) + " ms<br/>\n";
  message += "Upper Average: " + String(myMachine->myTuner->upperAverage()) + " &deg;C<br/>\n";
  message += "Lower Average: " + String(myMachine->myTuner->lowerAverage()) + " &deg;C<br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"./tuningmode\"><button style=\"background-color:#7070EE\">Finish PID Tuning Mode</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"/config\"><button>Back</button></a><br/>\n";
  request->send(200, "text/html", message);
}

void WebInterface::handleSetConfig(AsyncWebServerRequest *request)
{
  bool reconf = false;
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head><h1>Configuration changed !</h1>\n";
  for (uint8_t i = 0; i < request->args(); i++)
  {
    if (request->argName(i) == "tset")
    {
      message += "new tset: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->targetTemp = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "tband")
    {
      message += "new tset: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->temperatureBand = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "epwr")
    {
      message += "new Eq. Pwr: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->eqPwr = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "pgain")
    {
      message += "new pgain: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->nearTarget.P = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "igain")
    {
      message += "new igain: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->nearTarget.I = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "dgain")
    {
      message += "new pgain: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->nearTarget.D = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "apgain")
    {
      message += "new pgain: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->awayTarget.P = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "aigain")
    {
      message += "new igain: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->awayTarget.I = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "adgain")
    {
      message += "new pgain: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->awayTarget.D = ((request->arg(i)).toDouble());
      reconf = true;
    }
#ifdef DEBUG
    else if (request->argName(i) == "PidInterval")
    {
      message += "new PidInterval: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->pidInt = ((request->arg(i)).toInt());
      reconf = true;
    }
    else if (request->argName(i) == "HeaterInterval")
    {
      message += "new HeaterInterval: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->heaterInterval = (abs((request->arg(i)).toInt()));
      reconf = true;
    }
    else if (request->argName(i) == "SensorSampleInterval")
    {
      message += "new Sensor Sample Interval: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->sensorSampleInterval = ((request->arg(i)).toInt());
      // reconfig not needed
    }
    else if (request->argName(i) == "maxCool")
    {
      message += "new Max Cool: " + request->arg(i) + "<br/>\n";
      myMachine->myConfig->maxCool = ((request->arg(i)).toDouble());
    }

#endif
  }
  if (reconf)
  {
    myMachine->reConfig(); // apply all settings
  }
  request->send(200, "text/html", message);
}

void WebInterface::handleSetTuning(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head><h1>Configuration changed !</h1>\n";
  for (uint8_t i = 0; i < request->args(); i++)
  {
    if (request->argName(i) == "tunethres")
    {
      message += "new tuning threshold: " + request->arg(i) + "<br/>\n";
      myMachine->myTuner->setTuneThres(((request->arg(i)).toDouble()));
    }
    else if (request->argName(i) == "tunestep")
    {
      message += "new tuning power: " + request->arg(i) + "<br/>\n";
      myMachine->myTuner->setTuneStep((request->arg(i)).toDouble());
    }
  }
  request->send(200, "text/html", message);
}

void WebInterface::handleLoadConfig(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if (myMachine->myConfig->loadConfig())
  {
    myMachine->reConfig();
    message += "<h1>Configuration loaded !</h1>\n";
  }
  else
    message += "<h1>Error loading configuration !</h1>\n";
  request->send(200, "text/html", message);
}

void WebInterface::handleSaveConfig(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if (myMachine->myConfig->saveConfig())
    message += "<h1>Configuration saved !</h1>\n";
  else
    message += "<h1>Error saving configuration !</h1>\n";
  request->send(200, "text/html", message);
}

void WebInterface::handleResetConfig(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  myMachine->myConfig->resetConfig();
  myMachine->reConfig();
  message += "<h1>Configuration set to default !</h1>\n";
  request->send(200, "text/html", message);
}




void WebInterface::handlePidOn(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  myMachine->externalControlMode = false;
  request->send(200, "text/html", message);
}

void WebInterface::handlePidOff(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  myMachine->externalControlMode = true;
  request->send(200, "text/html", message);
}

void WebInterface::handleFile(AsyncWebServerRequest *request, const char *mimetype, const unsigned char *compressedData, const size_t compressedDataLen)
{
  AsyncWebServerResponse *response = request->beginResponse_P(200, mimetype, compressedData, compressedDataLen);
  response->addHeader("Server", "ESP Async Web Server");
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}

void WebInterface::handleTuningMode(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if (!myMachine->tuning)
  {
    myMachine->tuning = myMachine->myTuner->tuning_on();

    message += "<h1> Started ! </h1>";
  }
  else
  {
    message = "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
    myMachine->tuning = myMachine->myTuner->tuning_off();
    message += "<h1> Finished and new parameters calculated ! </h1>";
    message += "Total power-on-cycles: " + String(myMachine->myTuner->getTuneCount()) + "<br/>\n";
    message += "Average Period: " + String(myMachine->myTuner->averagePeriod()) + " ms<br/>\n";
    message += "Average Peak-To-Peak Temperature: " + String(myMachine->myTuner->averagePeakToPeak()) + " &deg;C<br/>\n";
    message += "<a href=\"/config\"><button>Back</button></a><br/>\n";
  }

  request->send(200, "text/html", message);
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
  server->on("/loadconf", HTTP_GET, std::bind(&WebInterface::handleLoadConfig, this, std::placeholders::_1));
  server->on("/saveconf", HTTP_GET, std::bind(&WebInterface::handleSaveConfig, this, std::placeholders::_1));
  server->on("/resetconf", HTTP_GET, std::bind(&WebInterface::handleResetConfig, this, std::placeholders::_1));
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