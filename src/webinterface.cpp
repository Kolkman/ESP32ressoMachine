//
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// Web Server with Options and Plotter
//

#include "ESPressiot.h"
#include <PID_v1.h>

#include "web.h"
#include "tuning.h"
#include "config.h"


#ifdef ENABLE_HTTP

#include <WebServer.h>
#include <HTTPUpdateServer.h>

#define ONCOLOR "CD212A"
#define OFFCOLOR "DCDCDC"
 


WebServer server(80);
HTTPUpdateServer httpUpdater;

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void handleRoot() {
  String powerOnColor = String(ONCOLOR);
  String powerOffColor = String(OFFCOLOR);
  String pidModeColor = "98B4D4";
  String extModeColor = "DCDCDC";
  
  if (poweroffMode) {
    powerOnColor = String(OFFCOLOR);
    powerOffColor = String(ONCOLOR);
  } 
  if (externalControlMode) {
    extModeColor = pidModeColor;
    pidModeColor = "DCDCDC";
  } 
  
  String message = "<head><meta http-equiv=\"refresh\" content=\"2\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n<title>EspressIoT</title></head><h1>EspressIoT</h1>\n";
  message += "<hr/>";
  message += "<h2>Status<br/>\n";
  message += "<h3>Measured Temperature: " + String(gInputTemp) + "<br/>\n";
  message += "Target Temperature: " + String(gTargetTemp) + "<br/>\n";
  message += "Heater Power: " + String(gOutputPwr) + "</h3>\n";
  message += "<hr/>";
  message += "<h2> Operational Mode (PID / external trigger) </h2>";
  message += "<a href=\"./pid_on\"><button style=\"background-color:#"+pidModeColor+"\">PID</button></a>\n";
  message += "<a href=\"./pid_off\"><button style=\"background-color:#"+extModeColor+"\">external trigger</button></a><br/>\n";
  message += "<hr/>\n";
  //if (poweroffMode) message += "<a href=\"./toggleheater\"><button style=\"background-color:#FF0000\">Toggle Heater</button></a><br/>\n";
  //else message += "<a href=\"./toggleheater\"><button style=\"background-color:#00FF00\">Toggle Heater</button></a><br/>\n";
  message += "<a href=\"./heater_on\"><button style=\"background-color:#"+powerOnColor+"\">Turn Heater On</button></a>\n";
  message += "<a href=\"./heater_off\"><button style=\"background-color:#"+powerOffColor+"\">Turn Heater Off</button></a><br/>\n";  
  message += "<a href=\"./config\"><button>Settings</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<font size=\"-2\">Firmware: " + String(CURRENTFIRMWARE) + "-"+String(F(__DATE__))+":"+String(F(__TIME__))+"</font><br/>\n";

  message += "<hr/>\n";


  server.send(200, "text/html", message);
}

void handleConfig() {
  String powerOnColor = String(ONCOLOR);
  String powerOffColor = String(OFFCOLOR);
  if (poweroffMode) {
    powerOnColor = String(OFFCOLOR);
    powerOffColor = String(ONCOLOR);
  } 
  
  String message = "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT Configuration</title></head><h1>EspressIoT</h1>\n";
  if (tuning) {
    message += "<h1> PID TUNING MODE RUNNING !</h1>";
    message += "<a href=\"/tuningstats\"><button>Stats</button></a><br/>\n";
    message += "<hr/>\n";
  }
  message += "<form action=\"set_config\">\nTarget Temperature:<br>\n";
  message += "<input type=\"text\" name=\"tset\" value=\"" + String(gTargetTemp) + "\"><br/><br/>\n";
  message += "<form action=\"set_config\">\nThreshold for PID control:<br>\n";
  message += "<input type=\"text\" name=\"tband\" value=\"" + String(gOvershoot) + "\"><br/><br/>\n";
  message += "<form action=\"set_config\">\nEstimated power to maintain Equilibrium at "+String(gTargetTemp)+":<br>\n";
  message += "<input type=\"text\" name=\"epwr\" value=\"" + String(gEqPwr) + "\"><br/><br/>\n";
  message += "PID within " + String(gTargetTemp-gOvershoot) + " to "+ String(gTargetTemp+gOvershoot) + " band:<br>\n P <input type=\"text\" name=\"pgain\" value=\"" + String(gP) + "\"><br/>\n";
  message += "I <input type=\"text\" name=\"igain\" value=\"" + String(gI) + "\"><br/>\n";
  message += "D <input type=\"text\" name=\"dgain\" value=\"" + String(gD) + "\"><br><br>\n";
  message += "PID from oudside the band:<br>\n P <input type=\"text\" name=\"apgain\" value=\"" + String(gaP) + "\"><br/>\n";
  message += "I <input type=\"text\" name=\"aigain\" value=\"" + String(gaI) + "\"><br/>\n";
  message += "D <input type=\"text\" name=\"adgain\" value=\"" + String(gaD) + "\"><br><br>\n";
#ifdef DEBUG
  message += "Pid Interval <input type=\"text\" name=\"PidInterval\" value=\"" + String(gPIDint) + "\"><br><br>\n";
  message += "Heater Interval <input type=\"text\" name=\"HeaterInterval\" value=\"" + String(heaterDriver.getHeaterInterval()) + "\"><br><br>\n"; 
  message += "MAX Sample <input type=\"text\" name=\"MAXSampleInterval\" value=\"" + String(gMAXsample) + "\"><br><br>\n"; 
#endif
  message += "<input type=\"submit\" value=\"Submit\">\n</form>";

  message += "<hr/>";

  message += "<a href=\"./heater_on\"><button style=\"background-color:#"+powerOnColor+"\">Turn Heater On</button></a>\n";
  message += "<a href=\"./heater_off\"><button style=\"background-color:#"+powerOffColor+"\">Turn Heater Off</button></a><br/>\n";  
  message += "<hr/>";
  message += "<a href=\"./loadconf\"><button>Load Config</button></a><br/>\n";
  message += "<a href=\"./saveconf\"><button>Save Config</button></a><br/>\n";
  message += "<a href=\"./resetconf\"><button>Reset Config to Default</button></a><br/>\n";
  message += "<a href=\"./update\"><button>Update Firmware</button></a><br/>\n";
  message += "<a href=\"./reset\"><button>Reset Device</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<form action=\"set_tuning\">\nTuning Threshold (°C):<br>\n";
  message += "<input type=\"text\" name=\"tunethres\" value=\"" + String(aTuneThres) + "\"><br>\n";
  message += "Tuning Power (heater)<br>\n";
  message += "<input type=\"text\" name=\"tunestep\" value=\"" + String(aTuneStep) + "\"><br><br>\n";
  message += "<input type=\"submit\" value=\"Submit\">\n</form><br/>";
  if (!tuning) message += "<a href=\"./tuningmode\"><button style=\"background-color:#98B4D4\">Start PID Tuning Mode</button></a><br/>\n";
  else message += "<a href=\"./tuningmode\"><button style=\"background-color:#98B4D4\">Finish PID Tuning Mode</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"/\"><button>Back</button></a><br/>\n";
    message += "<hr/>\n";
  message += "<font size=\"-2\">Firmware: " + String(CURRENTFIRMWARE) + "-"+String(F(__DATE__))+":"+String(F(__TIME__))+"</font><br/>\n";

  message += "<hr/>\n";

  server.send(200, "text/html", message);
}

void handleTuningStats() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"5\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT Configuration</title></head><h1>EspressIoT</h1>\n";
  message += "<h1> PID TUNING STATS </h1>";
  message += "Total power-on-cycles: " + String(tune_count) + "<br/>\n";
  message += "Time elapsed: " + String(tune_time - tune_start) + " ms<br/>\n";
  message += "Average Period: " + String( float(tune_time - tune_start) / tune_count) + " ms<br/>\n";
  message += "Upper Average: " + String(AvgUpperT / UpperCnt) + " °C<br/>\n";
  message += "Lower Average: " + String(AvgLowerT / LowerCnt) + " °C<br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"./tuningmode\"><button style=\"background-color:#7070EE\">Finish PID Tuning Mode</button></a><br/>\n";
  message += "<hr/>\n";
  message += "<a href=\"/config\"><button>Back</button></a><br/>\n";
  server.send(200, "text/html", message);
}

void handleSetConfig() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head><h1>Configuration changed !</h1>\n";
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName(i) == "tset") {
      message += "new tset: " + server.arg ( i ) + "<br/>\n";
      gTargetTemp = ( (server.arg(i)).toFloat() );
    }
    else if (server.argName(i) == "tband") {
      message += "new tset: " + server.arg ( i ) + "<br/>\n";
      gOvershoot = ( (server.arg(i)).toFloat() );
    }
        else if (server.argName(i) == "epwr") {
      message += "new Eq. Pwr: " + server.arg ( i ) + "<br/>\n";
      gEqPwr = ( (server.arg(i)).toFloat() );
    }
    else if (server.argName(i) == "pgain") {
      message += "new pgain: " + server.arg ( i ) + "<br/>\n";
      gP = ( (server.arg(i)).toFloat() );
    }
    else if (server.argName(i) == "igain") {
      message += "new igain: " + server.arg ( i ) + "<br/>\n";
      gI = ( (server.arg(i)).toFloat() );
    }
    else if (server.argName(i) == "dgain") {
      message += "new pgain: " + server.arg ( i ) + "<br/>\n";
      gD = ( (server.arg(i)).toFloat() );
    }
    else if (server.argName(i) == "apgain") {
      message += "new pgain: " + server.arg ( i ) + "<br/>\n";
      gaP = ( (server.arg(i)).toFloat() );
    }
    else if (server.argName(i) == "aigain") {
      message += "new igain: " + server.arg ( i ) + "<br/>\n";
      gaI = ( (server.arg(i)).toFloat() );
    }
    else if (server.argName(i) == "adgain") {
      message += "new pgain: " + server.arg ( i ) + "<br/>\n";
      gaD = ( (server.arg(i)).toFloat() );
    }
#ifdef DEBUG
    else if (server.argName(i) == "PidInterval") {
      message += "new PidInterval: " + server.arg ( i ) + "<br/>\n";
      gPIDint = ( (server.arg(i)).toFloat() );
       espPid.SetSampleTime(gPIDint); 
    }
    else if (server.argName(i) == "HeaterInterval") {
      message += "new HeaterInterval: " + server.arg ( i ) + "<br/>\n";
      heaterDriver.setHeaterInterval( (server.arg(i)).toFloat() );
    }
    else if (server.argName(i) == "MAXSampleInterval") {
      message += "new MAXSampleInterval: " + server.arg ( i ) + "<br/>\n";
      gMAXsample = ( (server.arg(i)).toFloat() );
    }
 
#endif  
   if ( abs(gTargetTemp - gInputTemp) >= gOvershoot ) {
          espPid.SetTunings(gaP, gaI, gaD,P_ON_E);
   }else{
           //force reinitialize PID at equibrilibrium-power (manually determined)
        
        espPid.SetTunings(gP, gI, gD,P_ON_M); 
       
   }
  }
  server.send(200, "text/html", message);

}

void handleSetTuning() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head><h1>Configuration changed !</h1>\n";
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName(i) == "tunethres") {
      message += "new tuning threshold: " + server.arg ( i ) + "<br/>\n";
      aTuneThres = ( (server.arg(i)).toFloat() );
    }
    else if (server.argName(i) == "tunestep") {
      message += "new tuning power: " + server.arg ( i ) + "<br/>\n";
      aTuneStep = ( (server.arg(i)).toFloat() );
    }
  }
  server.send(200, "text/html", message);

}

void handleLoadConfig() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if (loadConfig()) message += "<h1>Configuration loaded !</h1>\n";
  else message += "<h1>Error loading configuration !</h1>\n";
  server.send(200, "text/html", message);
}

void handleSaveConfig() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if (saveConfig()) message += "<h1>Configuration saved !</h1>\n";
  else message += "<h1>Error saving configuration !</h1>\n";
  server.send(200, "text/html", message);
}

void handleResetConfig() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  resetConfig();
  message += "<h1>Configuration set to default !</h1>\n";
  server.send(200, "text/html", message);
}

void handleToggleHeater() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  poweroffMode = (!poweroffMode);
  server.send(200, "text/html", message);
}

void handleHeaterSwitch(boolean newMode) {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  poweroffMode = (newMode);
  server.send(200, "text/html", message);
}

void handleHeaterOn() {
  handleHeaterSwitch(false);
}

void handleHeaterOff() {
  handleHeaterSwitch(true);
}

void handlePidOn() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  externalControlMode = false;
  server.send(200, "text/html", message);
}

void handlePidOff() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Done ! </h1>";
  externalControlMode = true;
  server.send(200, "text/html", message);
}

void handleTuningMode() {
  String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/config\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  if (!tuning) {
    tuning_on();
    message += "<h1> Started ! </h1>";
  }
  else {
    message = "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
    tuning_off();
    message += "<h1> Finished and new parameters calculated ! </h1>";
    message += "Total power-on-cycles: " + String(tune_count) + "<br/>\n";
    message += "Average Period: " + String( float(tune_time - tune_start) / tune_count) + " ms<br/>\n";
    message += "Average Peak-To-Peak Temperature: " + String((AvgUpperT / UpperCnt) - (AvgLowerT / LowerCnt)) + " °C<br/>\n";
    message += "<a href=\"/config\"><button>Back</button></a><br/>\n";
  }

  server.send(200, "text/html", message);
}


void handleReset() {
   String message = "<head><meta http-equiv=\"refresh\" content=\"2;url=/\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>EspressIoT</title></head>";
  message += "<h1> Reseting Device ! </h1>";
  server.send(200, "text/html", message);
   delay(1000);
  ESP.restart();
}

void setupWebSrv() {

  httpUpdater.setup(&server);
  Serial.print("Updater running !");
  server.on("/", handleRoot);
  server.on("/config", handleConfig);
  server.on("/loadconf", handleLoadConfig);
  server.on("/saveconf", handleSaveConfig);
  server.on("/resetconf", handleResetConfig);
  server.on("/set_config", handleSetConfig);
  server.on("/tuningmode", handleTuningMode);
  server.on("/tuningstats", handleTuningStats);
  server.on("/set_tuning", handleSetTuning);
  server.on("/heater_on", handleHeaterOn);
  server.on("/heater_off", handleHeaterOff);
  server.on("/pid_on", handlePidOn);
  server.on("/pid_off", handlePidOff);
  server.onNotFound ( handleNotFound );
    server.on("/reset", handleReset);  
  server.begin();
  Serial.println("HTTP server started");
}

void loopWebSrv() {
  server.handleClient();
}

#endif