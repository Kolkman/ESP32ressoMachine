
// Code fragments from
// https://github.com/lbernstone/asyncUpdate/bb/master/AsyncUpdate.ino

#include "webInterfaceAPI.h"
#include <ESPAsyncWebServer.h>

webInterfaceAPI webAPI;

webInterfaceAPI::webInterfaceAPI()
{
  server = nullptr;
  myMachine = nullptr;
}

void webInterfaceAPI::begin(EspressoWebServer *s, ESPressoMachine *m)
{
  server = s;
  myMachine = m;
  if (myMachine == nullptr)
  {
    Serial.println("_Machine is a null pointer");
    throw("webInterfaceAPI::begin ESPresspMachine * is a NULL PTR");
  }
  server->on("/api/v1/status", HTTP_GET, std::bind(&webInterfaceAPI::handleStatus, this, std::placeholders::_1));
  server->on("/api/v1/firmware", HTTP_GET, std::bind(&webInterfaceAPI::handleFirmware, this, std::placeholders::_1));
  server->on("/api/v1/get", HTTP_GET, std::bind(&webInterfaceAPI::handleGet, this, std::placeholders::_1));
  server->on("/api/v1/set", HTTP_GET, std::bind(&webInterfaceAPI::handleSet, this, std::placeholders::_1));
  server->on("/api/v1/statistics", HTTP_GET, std::bind(&webInterfaceAPI::handleStats, this, std::placeholders::_1));
}

void webInterfaceAPI::handleStatus(AsyncWebServerRequest *request)
{
  myMachine->setMachineStatus();
  request->send(200, "application/json", myMachine->machineStatus);
}

void webInterfaceAPI::handleFirmware(AsyncWebServerRequest *request)
{
  String message = "{\"version\": \"" + String(CURRENTFIRMWARE) + "-" + String(F(__DATE__)) + ":" + String(F(__TIME__)) + "\"}";
  request->send(200, "application/json", message);
}

void addjson(char *msg, bool &firstarg, String argument, String value)
{
  if (!firstarg)
    strcat(msg, ",");
  else
    firstarg = false;
  char addition[50];
  ("\"" + argument + "\":" + value).toCharArray(addition, 50);
  strcat(msg, addition);
  return;
}

void webInterfaceAPI::handleStats(AsyncWebServerRequest *request)
{
  request->send(200, "application/json", myMachine->getStatistics());
};

void webInterfaceAPI::handleGet(AsyncWebServerRequest *request)
{
  bool firstarg = true;
  char message[2048]; // This is sufficiently big.
  strcpy(message, "{");

  for (uint8_t i = 0; i < request->args(); i++)
  {
    if (request->argName(i) == "tset")
    {
      addjson(message, firstarg, "tset",
              String(myMachine->myConfig->targetTemp));
    }
    else if (request->argName(i) == "tband")
    {
      addjson(message, firstarg, "tband",
              String(myMachine->myConfig->temperatureBand));
    }

    else if (request->argName(i) == "eqPwr")
    {
      addjson(message, firstarg, "eqPwr",
              String(myMachine->myConfig->eqPwr));
    }
    else if (request->argName(i) == "pgain")
    {
      addjson(message, firstarg, "pgain",
              String(myMachine->myConfig->nearTarget.P));
    }
    else if (request->argName(i) == "igain")
    {
      addjson(message, firstarg, "igain",
              String(myMachine->myConfig->nearTarget.I));
    }
    else if (request->argName(i) == "dgain")
    {
      addjson(message, firstarg, "dgain",
              String(myMachine->myConfig->nearTarget.D));
    }
    else if (request->argName(i) == "apgain")
    {
      addjson(message, firstarg, "apgain",
              String(myMachine->myConfig->awayTarget.P));
    }
    else if (request->argName(i) == "aigain")
    {
      addjson(message, firstarg, "aigain",
              String(myMachine->myConfig->awayTarget.I));
    }
    else if (request->argName(i) == "adgain")
    {
      addjson(message, firstarg, "adgain",
              String(myMachine->myConfig->awayTarget.D));
    }
    else if (request->argName(i) == "powerOffMode")
    {
      char msg[6];
      if (myMachine->powerOffMode)
        strcpy(msg, "true");
      else
        strcpy(msg, "false");
      addjson(message, firstarg, "powerOffMode", msg);
    }
    else if (request->argName(i) == "externalControlMode")
    {
      char msg[6];
      if (myMachine->externalControlMode)
        strcpy(msg, "true");
      else
        strcpy(msg, "false");

      addjson(message, firstarg, "externalControlMode", msg);
    }
    else if (request->argName(i) == "PidInterval")
    {
      addjson(message, firstarg, "PidInterval",
              String(myMachine->myConfig->pidInt));
    }
    else if (request->argName(i) == "HeaterInterval")
    {
      addjson(message, firstarg, "HeaterInterval",
              String(myMachine->myConfig->heaterInterval));
    }
    else if (request->argName(i) == "sensorSampleInterval")
    {
      addjson(message, firstarg, "sensorSampleInterval",
              String(myMachine->myConfig->sensorSampleInterval));
    }
    else if (request->argName(i) == "maxCool")
    {
      addjson(message, firstarg, "maxCool",
              String(myMachine->myConfig->maxCool));
    }
    else if (request->argName(i) == "tunethres")
    {
      addjson(message, firstarg, "tunethres",
              String(myMachine->myTuner->getTuneThres()));
    }
    else if (request->argName(i) == "tunestep")
    {
      addjson(message, firstarg, "tunestep",
              String(myMachine->myTuner->getTuneStep()));
    }
  }
  strcat(message, "}");
  request->send(200, "application/json", message);
}
void webInterfaceAPI::handleSet(AsyncWebServerRequest *request)
{
  bool reconf = false;
  bool firstarg = true;

  char message[2048]; // This is sufficiently big to store all key:val combinations
  strcpy(message, "{");
  for (uint8_t i = 0; i < request->args(); i++)
  {
    if (request->argName(i) == "tset")
    {
      server->authenticate(request);
      double t = (request->arg(i)).toDouble();
      if (t > MAXTEMP)
        t = MAXTEMP; // bit of a safety thing
      myMachine->myConfig->targetTemp = (t);
      addjson(message, firstarg, "tset", String(t));
    }
    else if (request->argName(i) == "tband")
    {
      addjson(message, firstarg, "tband", request->arg(i));
      myMachine->myConfig->temperatureBand = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "eqPwr")
    {
      addjson(message, firstarg, "eqPwr", request->arg(i));
      myMachine->myConfig->eqPwr = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "pgain")
    {
      addjson(message, firstarg, "pgain", request->arg(i));
      myMachine->myConfig->nearTarget.P = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "igain")
    {
      addjson(message, firstarg, "igain", request->arg(i));
      myMachine->myConfig->nearTarget.I = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "dgain")
    {
      addjson(message, firstarg, "dgain", request->arg(i));
      myMachine->myConfig->nearTarget.D = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "apgain")
    {
      addjson(message, firstarg, "apgain", request->arg(i));
      myMachine->myConfig->awayTarget.P = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "aigain")
    {
      addjson(message, firstarg, "aigain", request->arg(i));
      myMachine->myConfig->awayTarget.I = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "adgain")
    {
      addjson(message, firstarg, "adgain", request->arg(i));
      myMachine->myConfig->awayTarget.D = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "powerOffMode")
    {
      if (String("true").equalsIgnoreCase(request->arg(i)))
      {
        myMachine->powerOffMode = true;
        addjson(message, firstarg, "powerOffMode", "true");
      }
      else
      {
        myMachine->powerOffMode = false;
        addjson(message, firstarg, "powerOffMode", "false");
      }
    }

    else if (request->argName(i) == "externalControlMode")
    {
      if (String("true").equalsIgnoreCase(request->arg(i)))
      {
        myMachine->externalControlMode = true;
        addjson(message, firstarg, "externalControlMode", "true");
      }
      else
      {
        myMachine->externalControlMode = false;
        addjson(message, firstarg, "externalControlMode", "false");
      }
    }

    else if (request->argName(i) == "PidInterval")
    {
      addjson(message, firstarg, "PidInterval", request->arg(i));
      myMachine->myConfig->pidInt = ((request->arg(i)).toInt());
      reconf = true;
    }
    else if (request->argName(i) == "HeaterInterval")
    {
      addjson(message, firstarg, "HeaterInterval", request->arg(i));
      myMachine->myConfig->heaterInterval = (abs((request->arg(i)).toInt()));
      reconf = true;
    }
    else if (request->argName(i) == "sensorSampleInterval")
    {
      addjson(message, firstarg, "sensorSampleInterval", request->arg(i));
      myMachine->myConfig->sensorSampleInterval = ((request->arg(i)).toInt());
      // reconfig not needed
    }
    else if (request->argName(i) == "maxCool")
    {
      addjson(message, firstarg, "maxCool", request->arg(i));
      myMachine->myConfig->maxCool = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "tunethres")
    {
      addjson(message, firstarg, "tunethres", request->arg(i));

      myMachine->myTuner->setTuneThres((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "tunestep")
    {
      addjson(message, firstarg, "tunestep", request->arg(i));
      myMachine->myTuner->setTuneThres((request->arg(i)).toDouble());
      if ((request->arg(i)).toDouble() > 110)
      { // failsafe
        myMachine->myTuner->setTuneThres(110);
      }
    }
  }
  if (reconf)
  {
    myMachine->reConfig(); // apply all settings
  }
  strcat(message, "}");
  request->send(200, "application/json", message);
}
