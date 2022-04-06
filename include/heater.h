#ifndef ESPressiot_HEATER_H
#define ESPressiot_HEATER_H
#include <Arduino.h>

#define HEAT_RELAY_PIN 13 // 13 + GND are close on the ESP23 DEV Board
extern float heatcycles; // the number of millis out of 1000 for the current heat amount (percent * 10)
extern boolean heaterState;
extern unsigned long heatCurrentTime, heatLastTime;

#ifndef SIMULATION_MODE
void setupHeater();
void updateHeater();
#endif

void setHeatPowerPercentage(float);
float getHeatCycles();
void _turnHeatElementOnOff(boolean);

#endif