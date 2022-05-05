//
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// Web Server with Options and Plotter
//

#include "ESPressoMachine.h"
#include <PID_v1.h>

#include "webinterface.h"
#include "config.h"
#include <Arduino.h>
#include "webInterfaceOTAUpdate.h"

// Various WebSources:
#include "pages/test.html.h"
#include "pages/button.css.h"
#include "pages/ESPresso.css.h"
#include "pages/gauge.min.js.h"
#include "pages/EspressoMachine.svg.h"
#include "pages/index.html.h"



#define ONCOLOR "CD212A"
#define OFFCOLOR "DCDCDC"

WebInterface::WebInterface()
{
  Serial.println("Webinterfce Constructor");

  server = nullptr;
  myMachine = nullptr;
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
  message +=  htmlFooter;
  request->send(404, "text/html", message);
}



void WebInterface::handleRoot(AsyncWebServerRequest *request)
{

  request->redirect("/index.html");
}

void WebInterface::handleConfig(AsyncWebServerRequest *request)
{
  String powerOnColor = String(ONCOLOR);
  String powerOffColor = String(OFFCOLOR);
  if (myMachine->powerOffMode)
  {
    powerOnColor = String(OFFCOLOR);
    powerOffColor = String(ONCOLOR);
  }

  String message = "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT Configuration</title></head><h1>EspressIoT</h1>\n";
  if (myMachine->tuning)
  {
    message += "<h1> PID TUNING MODE RUNNING !</h1>";
    message += "<a href=\"/tuningstats\"><button>Stats</button></a><br/>\n";
    message += "<hr/>\n";
  }
  message += "<form action=\"set_config\">\nTarget Temperature:<br>\n";
  message += "<input type=\"text\" name=\"tset\" value=\"" + String(myMachine->myConfig->targetTemp) + "\"><br/><br/>\n";
  message += "<form action=\"set_config\">\nThreshold for PID control:<br>\n";
  message += "<input type=\"text\" name=\"tband\" value=\"" + String(myMachine->myConfig->temperatureBand) + "\"><br/><br/>\n";
  message += "<form action=\"set_config\">\nEstimated power to maintain Equilibrium at " + String(myMachine->myConfig->targetTemp) + ":<br>\n";
  message += "<input type=\"text\" name=\"epwr\" value=\"" + String(myMachine->myConfig->eqPwr) + "\"><br/><br/>\n";
  message += "PID within " + String(myMachine->myConfig->targetTemp - myMachine->myConfig->temperatureBand) + " to " +
             String(myMachine->myConfig->targetTemp + myMachine->myConfig->temperatureBand) +
             " band:<br>\n P <input type=\"text\" name=\"pgain\" value=\"" +
             String(myMachine->myConfig->nearTarget.P) + "\"><br/>\n";
  message += "I <input type=\"text\" name=\"igain\" value=\"" + String(myMachine->myConfig->nearTarget.I) + "\"><br/>\n";
  message += "D <input type=\"text\" name=\"dgain\" value=\"" + String(myMachine->myConfig->nearTarget.D) + "\"><br><br>\n";
  message += "PID from oudside the band:<br>\n P <input type=\"text\" name=\"apgain\" value=\"" + String(myMachine->myConfig->awayTarget.P) + "\"><br/>\n";
  message += "I <input type=\"text\" name=\"aigain\" value=\"" + String(myMachine->myConfig->awayTarget.I) + "\"><br/>\n";
  message += "D <input type=\"text\" name=\"adgain\" value=\"" + String(myMachine->myConfig->awayTarget.D) + "\"><br><br>\n";
#ifdef DEBUG
  message += "Pid Interval <input type=\"text\" name=\"PidInterval\" value=\"" + String(myMachine->myConfig->pidInt) + "\"><br><br>\n";
  message += "Heater Interval <input type=\"text\" name=\"Heater Interval\" value=\"" + String(myMachine->myHeater->getHeaterInterval()) + "\"><br><br>\n";
  message += "Sensor Sample Interval Sample <input type=\"text\" name=\"SensorSampleInterval\" value=\"" + String(myMachine->myConfig->sensorSampleInterval) + "\"><br><br>\n";
  message += "Max Cool (natural cooling rate) <input type=\"text\" name=\"maxCool\" value=\"" + String(myMachine->myConfig->maxCool, 4) + "\"><br><br>\n";
#endif
  message += "<input type=\"submit\" value=\"Submit\">\n</form>";

  message += "<hr/>";

  message += "<a href=\"./heater_on\"><button style=\"background-color:#" + powerOnColor + "\">Turn Heater On</button></a>\n";
  message += "<a href=\"./heater_off\"><button style=\"background-color:#" + powerOffColor + "\">Turn Heater Off</button></a><br/>\n";
  message += "<hr/>";
  message += "<a href=\"./loadconf\"><button>Load Config</button></a><br/>\n";
  message += "<a href=\"./saveconf\"><button>Save Config</button></a><br/>\n";
  message += "<a href=\"./resetconf\"><button>Reset Config to Default</button></a><br/>\n";
  message += "<a href=\"./update\"><button>Update Firmware</button></a><br/>\n";
  message += "<a href=\"./reset\"><button>Reset Device</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<form action=\"set_tuning\">\nTuning Threshold (&deg;C):<br>\n";
  message += "<input type=\"text\" name=\"tunethres\" value=\"" + String(myMachine->myTuner->getTuneThres()) + "\"><br>\n";
  message += "Tuning Power (heater)<br>\n";
  message += "<input type=\"text\" name=\"tunestep\" value=\"" + String(myMachine->myTuner->getTuneStep()) + "\"><br><br>\n";
  message += "<input type=\"submit\" value=\"Submit\">\n</form><br/>";
  if (!myMachine->tuning)
    message += "<a href=\"./tuningmode\"><button style=\"background-color:#98B4D4\">Start PID Tuning Mode</button></a><br/>\n";
  else
    message += "<a href=\"./tuningmode\"><button style=\"background-color:#98B4D4\">Finish PID Tuning Mode</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"/\"><button>Back</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<font size=\"-2\">Firmware: " + String(CURRENTFIRMWARE) + "-" + String(F(__DATE__)) + ":" + String(F(__TIME__)) + "</font><br/>\n";

  message += "<hr/>\n";

  request->send(200, "text/html", message);
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

void WebInterface::handleToggleHeater(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  myMachine->powerOffMode = (!myMachine->powerOffMode);
  request->send(200, "text/html", message);
}

void WebInterface::handleHeaterSwitch(AsyncWebServerRequest *request, boolean newMode)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  myMachine->powerOffMode = (newMode);
  request->send(200, "text/html", message);
}

void WebInterface::handleHeaterOn(AsyncWebServerRequest *request)
{
  WebInterface::handleHeaterSwitch(request, false);
}

void WebInterface::handleHeaterOff(AsyncWebServerRequest *request)
{
  WebInterface::handleHeaterSwitch(request, true);
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



void WebInterface::handleFile(AsyncWebServerRequest *request, const char* mimetype, const unsigned char * compressedData, const size_t compressedDataLen)
{
  AsyncWebServerResponse *response = request->beginResponse_P(200, mimetype, compressedData , compressedDataLen);
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

void WebInterface::handleApiStatus(AsyncWebServerRequest *request)
{
  String message = myMachine->statusAsJson();
  request->send(200, "application/json", message);
}

void WebInterface::handleApiFirmware(AsyncWebServerRequest *request)
{
  String message = "{\"version\": \"" + String(CURRENTFIRMWARE) + "-" + String(F(__DATE__)) + ":" + String(F(__TIME__)) + "\"}";
  request->send(200, "application/json", message);
}

String addjson(bool &firstarg, String argument, String value)
{
  String msg = "";
  if (!firstarg)
    msg += ",";
  else
    firstarg = false;
  msg += "\"" + argument + "\":" + value;
  return msg;
}

void WebInterface::handleApiSet(AsyncWebServerRequest *request)
{
  bool reconf = false;
  bool firstarg = true;

  String message = "{";

  for (uint8_t i = 0; i < request->args(); i++)
  {
    if (request->argName(i) == "tset")
    {
      message += addjson(firstarg, "tset", request->arg(i));
      myMachine->myConfig->targetTemp = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "tband")
    {
      message += addjson(firstarg, "tband", request->arg(i));
      myMachine->myConfig->temperatureBand = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "epwr")
    {
      message += addjson(firstarg, "EqPwr", request->arg(i));
      myMachine->myConfig->eqPwr = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "pgain")
    {
      message += addjson(firstarg, "pgain", request->arg(i));
      myMachine->myConfig->nearTarget.P = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "igain")
    {
      message += addjson(firstarg, "igain", request->arg(i));
      myMachine->myConfig->nearTarget.I = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "dgain")
    {
      message += addjson(firstarg, "pgain", request->arg(i));
      myMachine->myConfig->nearTarget.D = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "apgain")
    {
      message += addjson(firstarg, "pgain", request->arg(i));
      myMachine->myConfig->awayTarget.P = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "aigain")
    {
      message += addjson(firstarg, "igain", request->arg(i));
      myMachine->myConfig->awayTarget.I = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "adgain")
    {
      message += addjson(firstarg, "pgain", request->arg(i));
      myMachine->myConfig->awayTarget.D = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "powerOffMode")
    {
      if (String("true").equalsIgnoreCase(request->arg(i)))
      {
        myMachine->powerOffMode = true;
        message += addjson(firstarg, "powerOffMode", "true");
      }
      else
      {
        myMachine->powerOffMode = false;
        message += addjson(firstarg, "powerOffMode", "false");
      }
    }

    else if (request->argName(i) == "externalControlMode")
    {
      if (String("true").equalsIgnoreCase(request->arg(i)))
      {
        myMachine->externalControlMode = true;
        message += addjson(firstarg, "externalControlMode", "true");
      }
      else
      {
        myMachine->externalControlMode = false;
        message += addjson(firstarg, "externalControlMode", "false");
      }
    }

#ifdef DEBUG
    else if (request->argName(i) == "PidInterval")
    {
      message += addjson(firstarg, "PidInterval", request->arg(i));
      myMachine->myConfig->pidInt = ((request->arg(i)).toInt());
      reconf = true;
    }
    else if (request->argName(i) == "HeaterInterval")
    {
      message += addjson(firstarg, "HeaterInterval", request->arg(i));
      myMachine->myConfig->heaterInterval = (abs((request->arg(i)).toInt()));
      reconf = true;
    }
    else if (request->argName(i) == "SensorSampleInterval")
    {
      message += addjson(firstarg, "Sensor Sample Interval", request->arg(i));
      myMachine->myConfig->sensorSampleInterval = ((request->arg(i)).toInt());
      // reconfig not needed
    }
    else if (request->argName(i) == "maxCool")
    {
      message += addjson(firstarg, "Max Cool", request->arg(i));
      myMachine->myConfig->maxCool = ((request->arg(i)).toDouble());
    }

#endif
  }
  if (reconf)
  {
    myMachine->reConfig(); // apply all settings
  }
  message += "}";
  request->send(200, "application/json", message);
}

void WebInterface::handleReset(AsyncWebServerRequest *request)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Reseting Device ! </h1>";
  request->send(200, "text/html", message);
  delay(1000);
  ESP.restart();
}

void WebInterface::setupWebSrv(ESPressoMachine *machine)
{
  myMachine = machine;
  Serial.println("Setting up  Webserver");
  server = new AsyncWebServer(80);
  if (server == nullptr)
    Serial.println("SERIAL NULLPTR");

  events = new AsyncEventSource("/events");
  if (events == nullptr)
    Serial.println("Events NULLPTR");

  // httpUpdater->setup(server);
  // Serial.print("Updater running !");

  server->on("/", HTTP_GET, std::bind(&WebInterface::handleRoot, this, std::placeholders::_1));
  server->on("/config", HTTP_GET, std::bind(&WebInterface::handleConfig, this, std::placeholders::_1));
  server->on("/loadconf", HTTP_GET, std::bind(&WebInterface::handleLoadConfig, this, std::placeholders::_1));
  server->on("/saveconf", HTTP_GET, std::bind(&WebInterface::handleSaveConfig, this, std::placeholders::_1));
  server->on("/resetconf", HTTP_GET, std::bind(&WebInterface::handleResetConfig, this, std::placeholders::_1));
  server->on("/set_config", HTTP_GET, std::bind(&WebInterface::handleSetConfig, this, std::placeholders::_1));
  server->on("/tuningmode", HTTP_GET, std::bind(&WebInterface::handleTuningMode, this, std::placeholders::_1));
  server->on("/tuningstats", HTTP_GET, std::bind(&WebInterface::handleTuningStats, this, std::placeholders::_1));
  server->on("/set_tuning", HTTP_GET, std::bind(&WebInterface::handleSetTuning, this, std::placeholders::_1));
  server->on("/heater_on", HTTP_GET, std::bind(&WebInterface::handleHeaterOn, this, std::placeholders::_1));
  server->on("/heater_off", HTTP_GET, std::bind(&WebInterface::handleHeaterOff, this, std::placeholders::_1));
  server->on("/pid_on", HTTP_GET, std::bind(&WebInterface::handlePidOn, this, std::placeholders::_1));
  server->on("/pid_off", HTTP_GET, std::bind(&WebInterface::handlePidOff, this, std::placeholders::_1));
  server->onNotFound(std::bind(&WebInterface::handleNotFound, this, std::placeholders::_1));
  server->on("/reset", HTTP_GET, std::bind(&WebInterface::handleReset, this, std::placeholders::_1));
  server->on("/api/v1/status", HTTP_GET, std::bind(&WebInterface::handleApiStatus, this, std::placeholders::_1));
  server->on("/api/v1/firmware", HTTP_GET, std::bind(&WebInterface::handleApiFirmware, this, std::placeholders::_1));
  server->on("/api/v1/set", HTTP_GET, std::bind(&WebInterface::handleApiSet, this, std::placeholders::_1));



  webOTAUpdate.begin(server, "admin", "silvia");
  //definitions generated by MAKE
DEF_HANDLE_test_html
DEF_HANDLE_index_html
DEF_HANDLE_button_css
DEF_HANDLE_ESPresso_css
DEF_HANDLE_gauge_min_js
DEF_HANDLE_EspressoMachine_svg

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

void WebInterface::eventLoop(String statusJson)
{

  Serial.println("Sending data");
  events->send(statusJson.c_str(), "status", millis());
}