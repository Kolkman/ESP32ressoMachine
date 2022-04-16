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

#ifdef ENABLE_HTTP

#define ONCOLOR "CD212A"
#define OFFCOLOR "DCDCDC"

WebInterface::WebInterface()
{
  Serial.println("Webinterfce Constructor");
  server = nullptr;
  myMachine = nullptr;
  httpUpdater = new HTTPUpdateServer;
}

WebInterface::~WebInterface()
{
  Serial.println("Webinterfce Destructor");
}

void WebInterface::handleNotFound()
{

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += (server->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";

  for (uint8_t i = 0; i < server->args(); i++)
  {
    message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
  }

  server->send(404, "text/plain", message);
}

void WebInterface::handleRoot()
{
  String powerOnColor = String(ONCOLOR);
  String powerOffColor = String(OFFCOLOR);
  String pidModeColor = "98B4D4";
  String extModeColor = "DCDCDC";

  if (myMachine->poweroffMode)
  {
    powerOnColor = String(OFFCOLOR);
    powerOffColor = String(ONCOLOR);
  }

  if (myMachine->externalControlMode)
  {
    extModeColor = pidModeColor;
    pidModeColor = "DCDCDC";
  }

  String message = "<head><meta http-equiv=\"refresh\" content=\"2\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n<title>EspressIoT</title></head><h1>EspressIoT</h1>\n";
  message += "<hr/>";
  message += "<h2>Status<br/>\n";
  message += "<h3>Measured Temperature: " + String(myMachine->inputTemp) + "<br/>\n";
  message += "Target Temperature: " + String(myMachine->myConfig->targetTemp) + "<br/>\n";
  message += "Heater Power: " + String(myMachine->outputPwr) + "</h3>\n";
  message += "<hr/>";
  message += "<h2> Operational Mode (PID / external trigger) </h2>";
  message += "<a href=\"./pid_on\"><button style=\"background-color:#" + pidModeColor + "\">PID</button></a>\n";
  message += "<a href=\"./pid_off\"><button style=\"background-color:#" + extModeColor + "\">external trigger</button></a><br/>\n";
  message += "<hr/>\n";
  // if (poweroffMode) message += "<a href=\"./toggleheater\"><button style=\"background-color:#FF0000\">Toggle Heater</button></a><br/>\n";
  // else message += "<a href=\"./toggleheater\"><button style=\"background-color:#00FF00\">Toggle Heater</button></a><br/>\n";
  message += "<a href=\"./heater_on\"><button style=\"background-color:#" + powerOnColor + "\">Turn Heater On</button></a>\n";
  message += "<a href=\"./heater_off\"><button style=\"background-color:#" + powerOffColor + "\">Turn Heater Off</button></a><br/>\n";
  message += "<a href=\"./config\"><button>Settings</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<font size=\"-2\">Firmware: " + String(CURRENTFIRMWARE) + "-" + String(F(__DATE__)) + ":" + String(F(__TIME__)) + "</font><br/>\n";

  message += "<hr/>\n";

  server->send(200, "text/html", message);
}

void WebInterface::handleConfig()
{
  String powerOnColor = String(ONCOLOR);
  String powerOffColor = String(OFFCOLOR);
  if (myMachine->poweroffMode)
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
  message += "<form action=\"set_tuning\">\nTuning Threshold (°C):<br>\n";
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

  server->send(200, "text/html", message);
}

void WebInterface::handleTuningStats()
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"5\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT Configuration</title></head><h1>EspressIoT</h1>\n";
  message += "<h1> PID TUNING STATS </h1>";
  message += "Total power-on-cycles: " + String(myMachine->myTuner->getTuneCount()) + "<br/>\n";
  message += "Time elapsed: " + String(myMachine->myTuner->timeElapsed()) + " ms<br/>\n";
  message += "Average Period: " + String(myMachine->myTuner->averagePeriod()) + " ms<br/>\n";
  message += "Upper Average: " + String(myMachine->myTuner->upperAverage()) + " °C<br/>\n";
  message += "Lower Average: " + String(myMachine->myTuner->lowerAverage()) + " °C<br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"./tuningmode\"><button style=\"background-color:#7070EE\">Finish PID Tuning Mode</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"/config\"><button>Back</button></a><br/>\n";
  server->send(200, "text/html", message);
}

void WebInterface::handleSetConfig()
{
  bool reconf = false;
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head><h1>Configuration changed !</h1>\n";
  for (uint8_t i = 0; i < server->args(); i++)
  {
    if (server->argName(i) == "tset")
    {
      message += "new tset: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->targetTemp = ((server->arg(i)).toDouble());
    }
    else if (server->argName(i) == "tband")
    {
      message += "new tset: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->temperatureBand = ((server->arg(i)).toDouble());
    }
    else if (server->argName(i) == "epwr")
    {
      message += "new Eq. Pwr: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->eqPwr = ((server->arg(i)).toDouble());
    }
    else if (server->argName(i) == "pgain")
    {
      message += "new pgain: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->nearTarget.P = ((server->arg(i)).toDouble());
      reconf = true;
    }
    else if (server->argName(i) == "igain")
    {
      message += "new igain: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->nearTarget.I = ((server->arg(i)).toDouble());
      reconf = true;
    }
    else if (server->argName(i) == "dgain")
    {
      message += "new pgain: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->nearTarget.D = ((server->arg(i)).toDouble());
      reconf = true;
    }
    else if (server->argName(i) == "apgain")
    {
      message += "new pgain: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->awayTarget.P = ((server->arg(i)).toDouble());
      reconf = true;
    }
    else if (server->argName(i) == "aigain")
    {
      message += "new igain: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->awayTarget.I = ((server->arg(i)).toDouble());
      reconf = true;
    }
    else if (server->argName(i) == "adgain")
    {
      message += "new pgain: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->awayTarget.D = ((server->arg(i)).toDouble());
      reconf = true;
    }
#ifdef DEBUG
    else if (server->argName(i) == "PidInterval")
    {
      message += "new PidInterval: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->pidInt = ((server->arg(i)).toInt());
      reconf = true;
    }
    else if (server->argName(i) == "HeaterInterval")
    {
      message += "new HeaterInterval: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->heaterInterval = (abs((server->arg(i)).toInt()));
      reconf = true;
    }
    else if (server->argName(i) == "SensorSampleInterval")
    {
      message += "new Sensor Sample Interval: " + server->arg(i) + "<br/>\n";
      myMachine->myConfig->sensorSampleInterval = ((server->arg(i)).toInt());
      // reconfig not needed
    }

#endif
  }
  if (reconf)
  {
    myMachine->reConfig(); // apply all settings
  }
  server->send(200, "text/html", message);
}

void WebInterface::handleSetTuning()
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head><h1>Configuration changed !</h1>\n";
  for (uint8_t i = 0; i < server->args(); i++)
  {
    if (server->argName(i) == "tunethres")
    {
      message += "new tuning threshold: " + server->arg(i) + "<br/>\n";
      myMachine->myTuner->setTuneThres(((server->arg(i)).toDouble()));
    }
    else if (server->argName(i) == "tunestep")
    {
      message += "new tuning power: " + server->arg(i) + "<br/>\n";
      myMachine->myTuner->setTuneStep((server->arg(i)).toDouble());
    }
  }
  server->send(200, "text/html", message);
}

void WebInterface::handleLoadConfig()
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if (myMachine->myConfig->loadConfig())
  {
    myMachine->reConfig();
    message += "<h1>Configuration loaded !</h1>\n";
  }
  else
    message += "<h1>Error loading configuration !</h1>\n";
  server->send(200, "text/html", message);
}

void WebInterface::handleSaveConfig()
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if (myMachine->myConfig->saveConfig())
    message += "<h1>Configuration saved !</h1>\n";
  else
    message += "<h1>Error saving configuration !</h1>\n";
  server->send(200, "text/html", message);
}

void WebInterface::handleResetConfig()
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  myMachine->myConfig->resetConfig();
  myMachine->reConfig();
  message += "<h1>Configuration set to default !</h1>\n";
  server->send(200, "text/html", message);
}

void WebInterface::handleToggleHeater()
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  myMachine->poweroffMode = (!myMachine->poweroffMode);
  server->send(200, "text/html", message);
}

void WebInterface::handleHeaterSwitch(boolean newMode)
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  myMachine->poweroffMode = (newMode);
  server->send(200, "text/html", message);
}

void WebInterface::handleHeaterOn()
{
  WebInterface::handleHeaterSwitch(false);
}

void WebInterface::handleHeaterOff()
{
  WebInterface::handleHeaterSwitch(true);
}

void WebInterface::handlePidOn()
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  myMachine->externalControlMode = false;
  server->send(200, "text/html", message);
}

void WebInterface::handlePidOff()
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  myMachine->externalControlMode = true;
  server->send(200, "text/html", message);
}

void WebInterface::handleTuningMode()
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if (!myMachine->tuning)
  {
    myMachine->myTuner->tuning_on();
    message += "<h1> Started ! </h1>";
  }
  else
  {
    message = "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
    myMachine->myTuner->tuning_off();
    message += "<h1> Finished and new parameters calculated ! </h1>";
    message += "Total power-on-cycles: " + String(myMachine->myTuner->getTuneCount()) + "<br/>\n";
    message += "Average Period: " + String(myMachine->myTuner->averagePeriod()) + " ms<br/>\n";
    message += "Average Peak-To-Peak Temperature: " + String(myMachine->myTuner->averagePeakToPeak()) + " °C<br/>\n";
    message += "<a href=\"/config\"><button>Back</button></a><br/>\n";
  }

  server->send(200, "text/html", message);
}

void WebInterface::handleReset()
{
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Reseting Device ! </h1>";
  server->send(200, "text/html", message);
  delay(1000);
  ESP.restart();
}

void WebInterface::setupWebSrv(ESPressoMachine *machine)
{
  Serial.println("Setting up  Webserver");
  server = new WebServer(80);
  myMachine = machine;
  if (server == nullptr)
    Serial.println("SERIAL NULLPTR");

  Serial.println("HIEROO");
  httpUpdater->setup(server);
  Serial.print("Updater running !");
  server->on("/", std::bind(&WebInterface::handleRoot, this));
  server->on("/config", std::bind(&WebInterface::handleConfig, this));
  server->on("/loadconf", std::bind(&WebInterface::handleLoadConfig, this));
  server->on("/saveconf", std::bind(&WebInterface::handleSaveConfig, this));
  server->on("/resetconf", std::bind(&WebInterface::handleResetConfig, this));
  server->on("/set_config", std::bind(&WebInterface::handleSetConfig, this));
  server->on("/tuningmode", std::bind(&WebInterface::handleTuningMode, this));
  server->on("/tuningstats", std::bind(&WebInterface::handleTuningStats, this));
  server->on("/set_tuning", std::bind(&WebInterface::handleSetTuning, this));
  server->on("/heater_on", std::bind(&WebInterface::handleHeaterOn, this));
  server->on("/heater_off", std::bind(&WebInterface::handleHeaterOff, this));
  server->on("/pid_on", std::bind(&WebInterface::handlePidOn, this));
  server->on("/pid_off", std::bind(&WebInterface::handlePidOff, this));
  server->onNotFound(std::bind(&WebInterface::handleNotFound, this));
  server->on("/reset", std::bind(&WebInterface::handleReset, this));

  server->begin();
  Serial.println("HTTP server started");
}

void WebInterface::loopWebSrv()
{
  server->handleClient();
}

#endif