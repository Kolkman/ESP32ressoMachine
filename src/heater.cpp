//
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// Simplified Heater PWM - just connect SSR to HEAT_RELAY_PIN
//
#include "heater.h"
#include <ESPressiot.h>

boolean heaterState = 0;
float heatcycles = 0;

unsigned long heatCurrentTime = 0, heatLastTime = 0;

#ifndef SIMULATION_MODE
void setupHeater() {
  pinMode(HEAT_RELAY_PIN , OUTPUT);
}

void updateHeater() {
  heatCurrentTime = time_now;
  if (heatCurrentTime - heatLastTime >=  gHEATERint or heatLastTime > heatCurrentTime) { //second statement prevents overflow errors
    // begin cycle
    _turnHeatElementOnOff(1);  //
    heatLastTime = heatCurrentTime;
  }
  if (heatCurrentTime - heatLastTime >= heatcycles) {
    _turnHeatElementOnOff(0);
  }
}
#endif

void setHeatPowerPercentage(float power) {
  if (power < 0.0) {
    power = 0.0;
  }
  if (power > 1000.0) {
    power = 1000.0;
  }
  //normalise heatcycle if the Heater interval is not 1000
  heatcycles = power * (gHEATERint/1000);
}

float getHeatCycles() {
  return heatcycles;
}

void _turnHeatElementOnOff(boolean on) {
  digitalWrite(HEAT_RELAY_PIN, on); //turn pin high
  heaterState = on;
}

// End Heater Control