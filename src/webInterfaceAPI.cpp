
// Code fragments from
// https://github.com/lbernstone/asyncUpdate/bb/master/AsyncUpdate.ino

#include "webInterfaceAPI.h"
#include <ESPAsyncWebServer.h>

webInterfaceAPI webAPI;

webInterfaceAPI::webInterfaceAPI()
{
  _server = nullptr;
  _machine = nullptr;
}

void webInterfaceAPI::begin(EspressoWebServer *server, ESPressoMachine *machine)
{
  _server = server;
  _machine = machine;
  if (_machine == nullptr)
  {
    Serial.println("_Machine is a null pointer");
    throw("webInterfaceAPI::begin ESPresspMachine * is a NULL PTR");
  }
  _server->on("/api/v1/status", HTTP_GET, std::bind(&webInterfaceAPI::handleStatus, this, std::placeholders::_1));
  _server->on("/api/v1/firmware", HTTP_GET, std::bind(&webInterfaceAPI::handleFirmware, this, std::placeholders::_1));
  _server->on("/api/v1/get", HTTP_GET, std::bind(&webInterfaceAPI::handleGet, this, std::placeholders::_1));
  _server->on("/api/v1/set", HTTP_GET, std::bind(&webInterfaceAPI::handleSet, this, std::placeholders::_1));
  _server->on("/api/v1/statistics", HTTP_GET, std::bind(&webInterfaceAPI::handleStats, this, std::placeholders::_1));
}

void webInterfaceAPI::handleStatus(AsyncWebServerRequest *request)
{
  String message = _machine->statusAsJson();
  request->send(200, "application/json", message);
}

void webInterfaceAPI::handleFirmware(AsyncWebServerRequest *request)
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

void webInterfaceAPI::handleStats(AsyncWebServerRequest *request)
{
  request->send(200, "application/json", _machine->getStatistics());
};

void webInterfaceAPI::handleGet(AsyncWebServerRequest *request)
{
  bool firstarg = true;
  String message = "{";
  for (uint8_t i = 0; i < request->args(); i++)
  {
    if (request->argName(i) == "tset")
    {
      message += addjson(firstarg, "tset",
                         String(_machine->myConfig->targetTemp));
    }
    else if (request->argName(i) == "tband")
    {
      message += addjson(firstarg, "tband",
                         String(_machine->myConfig->temperatureBand));
    }

    else if (request->argName(i) == "epwr")
    {
      message += addjson(firstarg, "EqPwr",
                         String(_machine->myConfig->eqPwr));
    }
    else if (request->argName(i) == "pgain")
    {
      message += addjson(firstarg, "pgain",
                         String(_machine->myConfig->nearTarget.P));
    }
    else if (request->argName(i) == "igain")
    {
      message += addjson(firstarg, "igain",
                         String(_machine->myConfig->nearTarget.I));
    }
    else if (request->argName(i) == "dgain")
    {
      message += addjson(firstarg, "pgain",
                         String(_machine->myConfig->nearTarget.D));
    }
    else if (request->argName(i) == "apgain")
    {
      message += addjson(firstarg, "pgain",
                         String(_machine->myConfig->awayTarget.P));
    }
    else if (request->argName(i) == "aigain")
    {
      message += addjson(firstarg, "igain",
                         String(_machine->myConfig->awayTarget.I));
    }
    else if (request->argName(i) == "adgain")
    {
      message += addjson(firstarg, "pgain",
                         String(_machine->myConfig->awayTarget.D));
    }
    else if (request->argName(i) == "powerOffMode")
    {
      char msg[6];
      if (_machine->powerOffMode) strcpy(msg,"true"); else strcpy(msg,"false");
      message += addjson(firstarg, "powerOffMode", msg);
    }
    else if (request->argName(i) == "externalControlMode")
    {
      char msg[6];
      if (_machine->externalControlMode) strcpy(msg,"true"); else strcpy(msg,"false");

      message += addjson(firstarg, "externalControlMode", msg );
    }
    else if (request->argName(i) == "PidInterval")
    {
      message += addjson(firstarg, "PidInterval",
                         String(_machine->myConfig->pidInt));
    }
    else if (request->argName(i) == "HeaterInterval")
    {
      message += addjson(firstarg, "HeaterInterval",
                         String(_machine->myConfig->heaterInterval));
    }
    else if (request->argName(i) == "SensorSampleInterval")
    {
      message += addjson(firstarg, "Sensor Sample Interval",
                         String(_machine->myConfig->sensorSampleInterval));
    }
    else if (request->argName(i) == "maxCool")
    {
      message += addjson(firstarg, "maxCool",
                         String(_machine->myConfig->maxCool));
    }
  }
  message += "}";
  request->send(200, "application/json", message);
}
void webInterfaceAPI::handleSet(AsyncWebServerRequest *request)
{
  bool reconf = false;
  bool firstarg = true;

  String message = "{";

  for (uint8_t i = 0; i < request->args(); i++)
  {

    if (request->argName(i) == "tset")
    {
      _server->authenticate(request);
      message += addjson(firstarg, "tset", request->arg(i));
      _machine->myConfig->targetTemp = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "tband")
    {
      message += addjson(firstarg, "tband", request->arg(i));
      _machine->myConfig->temperatureBand = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "epwr")
    {
      message += addjson(firstarg, "EqPwr", request->arg(i));
      _machine->myConfig->eqPwr = ((request->arg(i)).toDouble());
    }
    else if (request->argName(i) == "pgain")
    {
      message += addjson(firstarg, "pgain", request->arg(i));
      _machine->myConfig->nearTarget.P = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "igain")
    {
      message += addjson(firstarg, "igain", request->arg(i));
      _machine->myConfig->nearTarget.I = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "dgain")
    {
      message += addjson(firstarg, "pgain", request->arg(i));
      _machine->myConfig->nearTarget.D = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "apgain")
    {
      message += addjson(firstarg, "pgain", request->arg(i));
      _machine->myConfig->awayTarget.P = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "aigain")
    {
      message += addjson(firstarg, "igain", request->arg(i));
      _machine->myConfig->awayTarget.I = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "adgain")
    {
      message += addjson(firstarg, "pgain", request->arg(i));
      _machine->myConfig->awayTarget.D = ((request->arg(i)).toDouble());
      reconf = true;
    }
    else if (request->argName(i) == "powerOffMode")
    {
      if (String("true").equalsIgnoreCase(request->arg(i)))
      {
        _machine->powerOffMode = true;
        message += addjson(firstarg, "powerOffMode", "true");
      }
      else
      {
        _machine->powerOffMode = false;
        message += addjson(firstarg, "powerOffMode", "false");
      }
    }

    else if (request->argName(i) == "externalControlMode")
    {
      if (String("true").equalsIgnoreCase(request->arg(i)))
      {
        _machine->externalControlMode = true;
        message += addjson(firstarg, "externalControlMode", "true");
      }
      else
      {
        _machine->externalControlMode = false;
        message += addjson(firstarg, "externalControlMode", "false");
      }
    }

    else if (request->argName(i) == "PidInterval")
    {
      message += addjson(firstarg, "PidInterval", request->arg(i));
      _machine->myConfig->pidInt = ((request->arg(i)).toInt());
      reconf = true;
    }
    else if (request->argName(i) == "HeaterInterval")
    {
      message += addjson(firstarg, "HeaterInterval", request->arg(i));
      _machine->myConfig->heaterInterval = (abs((request->arg(i)).toInt()));
      reconf = true;
    }
    else if (request->argName(i) == "SensorSampleInterval")
    {
      message += addjson(firstarg, "Sensor Sample Interval", request->arg(i));
      _machine->myConfig->sensorSampleInterval = ((request->arg(i)).toInt());
      // reconfig not needed
    }
    else if (request->argName(i) == "maxCool")
    {
      message += addjson(firstarg, "Max Cool", request->arg(i));
      _machine->myConfig->maxCool = ((request->arg(i)).toDouble());
    }
  }
  if (reconf)
  {
    _machine->reConfig(); // apply all settings
  }
  message += "}";
  request->send(200, "application/json", message);
}
