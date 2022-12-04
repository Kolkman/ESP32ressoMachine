//
// ESPressIoT Controller for Espresso Machines
// (c) 2021 Olaf Kolkman
// Replaces senspr_max31855 (c) 2016-2021 by Roman Schmitz

// MAX31855 Sensor Arduino-MAX31855 Library

#ifndef SIMULATION_MODE
#include <Wire.h>
#include <SPI.h>
#include "sensor_max31855.h"

#undef DEBUG

TempSensor::TempSensor() : Adafruit_MAX31855(SENSOR_MAX_CLK, SENSOR_MAX_CS, SENSOR_MAX_DO)
{
  
  lastT = 0.0;
  SumT = 0.0;
  lastI = 0.0;
  SumI = 0.0;
  time_now=millis();
  TempCorrection = TEMP_CORRECTION;
  lastErr = 0.0;
  CntT = 0;
  CntI = 0;
  lastSensTime = 0;
}

void TempSensor::setupSensor()
{

  while (!Serial)
    delay(1); // wait for Serial on Leonardo/Zero, etc

  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.println("Initializing sensor...");
  if (!begin())
  {
    Serial.println("ERROR.");
    while (1)
      delay(10);
  }
  Serial.print("Measuring initial temperature");

  {
    int x = 0;
    double t = 0;
    for (int i = 0; i < 5; i++)
    {

      double c = readCelsius()+TempCorrection;
      if (isnan(c))
      {
        Serial.print("Thermocouple fault(s) detected!    @");
        Serial.println(time_now);

        uint8_t e = readError();
        if (e & MAX31855_FAULT_OPEN)
          Serial.println("FAULT: Thermocouple is open - no connections.");
        if (e & MAX31855_FAULT_SHORT_GND)
          Serial.println("FAULT: Thermocouple is short-circuited to GND.");
        if (e & MAX31855_FAULT_SHORT_VCC)
          Serial.println("FAULT: Thermocouple is short-circuited to VCC.");

      }
      else
      {
        t += c;
        x++;
      }
      Serial.print(".");
      delay(100);
    }
    Serial.println();
    lastT = t / x;
    Serial.println("Initial temperature: " + String(lastT));
    if (x == 0)
      throw("ThermoCoupleFail");
  }
}

void TempSensor::updateTempSensor(double sensorSampleInterval)
{

  time_now = millis();

  if ((max(time_now, lastSensTime) - min(time_now, lastSensTime)) >= sensorSampleInterval)
  {
   double i = readInternal();  
    double c = readCelsius()+TempCorrection;
    if (isnan(c) || isnan(i))
    {

      Serial.print("ThermoCouple Error");
      Serial.println(time_now);
    }
    else
    {
      double curT = c;
      double curI = i;
      // very simple selection of noise hits/invalid values
      // the weed-out vallue is rather high, to low will cause runnaway
      // heating cycles.
      if (abs(curT - lastT) < 15)
      {
        SumT += curT;
        lastT = curT;
        CntT++;
      }

      if (abs(curI - lastI) < 15 || lastI < 1)
      {
        SumI += curI;
        lastI = curI;
        CntI++;
      }

      lastSensTime = millis();
    }
  }
}

float TempSensor::getTemp(float temp)
{
  float retVal = temp; // default to return
  if (CntT >= 1)
  {
    retVal = (SumT / CntT);
    SumT = 0.;
    CntT = 0;
  }
  else
  {
#ifdef DEBUG
    Serial.println("No new temp measure");
#endif
  }
#ifdef DEBUG
  Serial.println("old/ret: " + String(temp) + "," + String(retVal));
#endif
  return retVal;
}

#endif
