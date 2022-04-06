//
// ESPressIoT Controller for Espresso Machines
// (c) 2021 Olaf Kolkman
// Replaces senspr_max31855 (c) 2016-2021 by Roman Schmitz

// MAX31855 Sensor Arduino-MAX31855 Library

#ifndef SIMULATION_MODE
#include <Wire.h>
#include <SPI.h>

#include <Adafruit_MAX31855.h>
#include <ESPressiot.h>
#include <sensor_max31855.h>



// Example creating a thermocouple instance with software SPI on three digital IO pins
#define MAXDO   26
#define MAXCS   27
#define MAXCLK  14

#define MAX31855_SMP_TIME 10


// initialize the Thermocouple
Adafruit_MAX31855 thermoc(MAXCLK, MAXCS, MAXDO);



double lastT = 0.0;
double SumT = 0.0;
double lastI = 0.0;
double SumI = 0.0;

double lastErr = 0.0;
int CntT = 0;
int CntI = 0;
unsigned long lastSensTime;

void setupSensor() {

  while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc

  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing sensor...");
  if (!thermoc.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
  Serial.println("DONE.");
}

void updateTempSensor() {

  time_now = millis();

  if ( (max(time_now, lastSensTime) - min(time_now, lastSensTime)) >= gMAXsample) {
    double i = thermoc.readInternal();
    double c = thermoc.readCelsius();
    if (isnan(c) || isnan(i) ){
      Serial.println("Could not get read temperature, Something wrong with thermocouple!");
    }
    else
    {
      double curT = c;
      double curI = i;
      // very simple selection of noise hits/invalid values 
      // the weed-out vallue is rather high, to low will cause runnaway
      // heating cycles.
      if (abs(curT - lastT) < 15 || lastT < 1) {
        SumT += curT;
        lastT = curT;
        CntT++;
      }
     if (abs(curI - lastI) < 15 || lastI < 1) {
        SumI += curI;
        lastI = curI;
        CntI++;
      }

      lastSensTime = millis();
    }
  }
}



float getTemp() {
  float retVal = gInputTemp;
  if (CntT >= 1) {
    retVal = (SumT / CntT);
    SumT = 0.;
    CntT = 0;
  }

  return retVal;
}


float getItemp() {
  float retVal = 0;
  if (CntI >= 1) {
    retVal = (SumI / CntI);
    SumI = 0.;
    CntI = 0;
  }

  return retVal;
}

#endif