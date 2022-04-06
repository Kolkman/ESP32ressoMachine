#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <PID_v1.h>

// local Includes
#include "ESPressiot.h"
#include "WiFiSecrets.h"
#include "config.h"
#include "helpers.h"

#include "telnet.h"
#include "web.h"
#include "heater.h"
#include "sensor_max31855.h"
#include "mqtt.h"
#include "tuning.h"


//
// global classes
PID ESPPID(&gInputTemp, &gOutputPwr, &gTargetTemp, gaP, gaI, gaD, P_ON_E, DIRECT);


/////////////////////////////////////////////////
// global variables (all declared in ESPressiot)

double gPIDint = PID_INTERVAL;
double gHEATERint = HEATER_INTERVAL;
double gEqPwr=EQUILIBRIUM_POWER;

double gMAXsample = 4;

double gTargetTemp = S_TSET;
double gOvershoot = S_TBAND;
double gInputTemp = 20.0;
double gOutputPwr = 0.0;
double gOldTemp =0.0;
double gOldPwr=0;
double gP = S_P, gI = S_I, gD = S_D;
double gaP = S_aP, gaI = S_aI, gaD = S_aD;

unsigned long time_now = 0;
unsigned long time_last = 0;

int gButtonState = 0;
uint8_t mac[6];

boolean tuning = false;
boolean osmode = false;
boolean coldstart = false;
boolean abovetarget = false;
boolean poweroffMode = false;
boolean externalControlMode = false;

String gStatusAsJson; 

void serialStatus() {
  Serial.println(gStatusAsJson);
}


void setup() {
  gOutputPwr = gEqPwr;

  Serial.begin(115200);
  // Need to print something to get the serial monitor setled
  for (int i = 0; i < 5 ; i++){
    Serial.print("Initializing (");
    Serial.print(i);
    Serial.println(")");
    delay(100);
  }

  
  Serial.println("Mounting SPIFFS...");
  if (!prepareFS()) {
    Serial.println("Failed to mount SPIFFS !");
  } else {
    Serial.println("Mounted.");
  }

  Serial.println("Loading config...");
  if (!loadConfig()) {
    Serial.println("Failed to load config. Using default values and creating config...");
    if (!saveConfig()) {
      Serial.println("Failed to save config");
    } else {
      Serial.println("Config saved");
    }
  } else {
    Serial.println("Config loaded");
  }

  Serial.println("Settin up PID...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.macAddress(mac);
  Serial.println("");
  Serial.print("MAC address: ");
  Serial.println(macToString(mac));


  Serial.print("Connecting to Wifi AP");
  for (int i = 0; i < MAX_CONNECTION_RETRIES && WiFi.status() != WL_CONNECTED; i++) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Error connection to AP after ");
    Serial.print(MAX_CONNECTION_RETRIES);
    Serial.println(" retries.");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   delay(2);
    Serial.println("something");

  

#ifdef ENABLE_TELNET
  setupTelnet();
#endif

#ifdef ENABLE_HTTP
  setupWebSrv();
#endif

#ifdef ENABLE_MQTT
  setupMQTT();
#endif

#ifdef ENABLE_SWITCH_DETECTION
  setupSwitch();
#endif

  // setup components
  setupHeater();
  setupSensor();

  // start PID
  ESPPID.SetTunings(gP, gI, gD);
  ESPPID.SetSampleTime(gPIDint);
  ESPPID.SetOutputLimits(0, 1000);
  ESPPID.SetMode(AUTOMATIC);
  gInputTemp = getTemp();
  coldstart = (gInputTemp < COLDSTART);
  osmode=(abs(gTargetTemp - gInputTemp) >= gOvershoot);
  time_now = millis();
  time_last = time_now;



}

void loop() {
  // put your main code here, to run repeatedly:

 time_now = millis();
  updateTempSensor();

#ifdef ENABLE_SWITCH_DETECTION
  loopSwitch();
#endif

  if ( (max(time_now, time_last) - min(time_now, time_last)) >= gPIDint or time_last > time_now) {

    gInputTemp = getTemp();


     //Maximum descent when high
//    if  (gInputTemp> (gTargetTemp-gOvershoot)) &&
    if  (!osmode  &&
      ((gOldTemp-gInputTemp) > (0.040 * gPIDint/1000)) ){
      gInputTemp=gOldTemp - 0.040*gPIDint/1000;
      gOldTemp=gInputTemp;
    }else{
      gOldTemp=gInputTemp;
    }

   
    Serial.println(gInputTemp);


  if (!coldstart && gInputTemp < 80){
      ESPPID.SetTunings(12.5, 0, 0,P_ON_E);
      ESPPID.SetOutputLimits(0,700);
      coldstart=true;
    } else if(coldstart && gInputTemp > 80){
         coldstart=false;
         osmode=(abs(gTargetTemp - gInputTemp) >= gOvershoot);
    }
   if (!coldstart &&  !osmode && abs(gTargetTemp - gInputTemp) >= gOvershoot ) {
          ESPPID.SetTunings(gaP, gaI, gaD,P_ON_E);
         
          if (gInputTemp < gTargetTemp)ESPPID.SetOutputLimits(0,500);
          if (gInputTemp > gTargetTemp)ESPPID.SetOutputLimits(0,30);
          osmode = true;  
  
   }
    // entering band    
    else if ( osmode && abs(gTargetTemp - gInputTemp) < gOvershoot ) {

        //force reinitialize PID at equibrilibrium-power (manually determined)
        
        ESPPID.SetMode(0);
        gOutputPwr=gEqPwr;  
        ESPPID.SetMode(1);
        
        ESPPID.SetTunings(gP, gI, gD,P_ON_E);
        ESPPID.SetOutputLimits(5,90);
        osmode = false;
    }
    if (!osmode && !abovetarget && (gInputTemp> gTargetTemp)){
        abovetarget=true;
   //     ESPPID.SetTunings(gP, gI, 0,P_ON_E);
    } else if (!osmode && abovetarget && (gInputTemp < gTargetTemp)){
       ESPPID.SetTunings(gP, gI, gD,P_ON_E);
       abovetarget=false;
    }
    if (osmode && !abovetarget && (gInputTemp> gTargetTemp)){
      abovetarget=true;
 //       ESPPID.SetTunings(gaP, gaI, 0,P_ON_E);
    } else if (osmode && abovetarget && (gInputTemp < gTargetTemp)){
       ESPPID.SetTunings(gaP, gaI, gaD,P_ON_E);
       abovetarget=false;
    }

   


    if (poweroffMode == true) {
      gOutputPwr = 0;
      setHeatPowerPercentage(gOutputPwr);
    }
    else if (externalControlMode == true) {
      gOutputPwr = 1000 * gButtonState;
      setHeatPowerPercentage(gOutputPwr);
    }
    else if (tuning == true)
    {
      tuning_loop();
    }
    // temperature well below the range where the PID kicks in.

    else if (ESPPID.Compute() == true) {
        // failsafe
       if (gInputTemp > 115) gOutputPwr=0;
//        // smooth thepower 
//        if ( (abs(gTargetTemp - gInputTemp) < gOvershoot)){
//          if (gOutputPwr> (gOldPwr+1.5 )){
//            gOutputPwr=gOldPwr+1.5;
//            gOldPwr=gOutputPwr;
//          }else if(gOutputPwr < (gOldPwr-1.5 )){
//            gOutputPwr=gOldPwr-1.5;
//            gOldPwr=gOutputPwr;
//          }
//        }
        setHeatPowerPercentage(gOutputPwr);
    }

    

    // create status String (JSON)
    gStatusAsJson = statusAsJson();
   

#ifdef ENABLE_MQTT
    loopMQTT();
#endif

#ifdef ENABLE_TELNET
    loopTelnet();
#endif

#ifdef ENABLE_SERIAL
    serialStatus();
#endif

    time_last = time_now;
  }

  updateHeater();







  #ifdef ENABLE_HTTP
    loopWebSrv();
  #endif  


}