//
// ESPressIoT Controller for Espresso Machines
// Initially based on code by
// 2016-2021 by Roman Schmitz
//
// Simplified Heater PWM - just connect SSR to HEAT_RELAY_PIN
//
// refactored by O. Kolkman
#include "Arduino.h"
#include "heater.h"



HEATER::HEATER(int pin, unsigned long i=1000, bool mode=false){
  heatCycles = 0;
  heaterState = false;
  heatCurrentTime = 0, heatLastTime = 0;
  heaterInterval=i;
  gpioPin=pin;
  simmulationMode=mode;
  if(! simmulationMode){
    pinMode(pin , OUTPUT);
  }

} 

void HEATER::setHeaterInterval(unsigned long interval){
  heaterInterval=interval;
}

unsigned long HEATER::getHeaterInterval(){
  return heaterInterval;
}

void HEATER::updateHeater(unsigned long t) {
  heatCurrentTime =  t;

  if (heatCurrentTime - heatLastTime >=  heaterInterval or heatLastTime > heatCurrentTime) { //second statement prevents overflow errors
    // begin cycle
    turnHeatElementOnOff(true);  //
    heatLastTime = heatCurrentTime;
  }
  if (heatCurrentTime - heatLastTime >= heatCycles) {
    turnHeatElementOnOff(false);
  }
}


void HEATER::setHeatPowerPercentage(float power) {
  if (power < 0.0) {
    power = 0.0;
  }
  if (power > 1000.0) {
    power = 1000.0;
  }
  //normalise heatcycle if the Heater interval is not 1000

  heatCycles = power * ((float) heaterInterval/ 1000.0);
  
}

float HEATER::getHeatCycles() {
  return heatCycles;
}

void HEATER::turnHeatElementOnOff(boolean on) {
  digitalWrite(gpioPin, on); //turn pin high
  heaterState = on;
}

// End Heater Control