// Based on work by 2016-2021 by Roman Schmitz
// Adopted and refactored by Olaf Kolkman, 2022.
//
//
// Simplified Heater PWM - just connect SSR to HEAT_RELAY_PIN
//
// refactored by O. Kolkman
#include "Arduino.h"
#include "heater.h"



Heater::Heater(int pin, unsigned long i, bool mode){
  init(pin,i,mode);
} 

Heater::Heater(){
  init(HEAT_RELAY_PIN,HEATER_INTERVAL,false);
}

void Heater::init(int pin, unsigned long i, bool mode)
{
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
void Heater::setHeaterInterval(unsigned long interval){
  heaterInterval=interval;
}

unsigned long Heater::getHeaterInterval(){
  return heaterInterval;
}

void Heater::updateHeater(unsigned long t) {
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


void Heater::setHeatPowerPercentage(float power) {
  if (power < 0.0) {
    power = 0.0;
  }
  if (power > 1000.0) {
    power = 1000.0;
  }
  //normalise heatcycle if the Heater interval is not 1000

  heatCycles = power * ((float) heaterInterval/ 1000.0);
  
}

float Heater::getHeatCycles() {
  return heatCycles;
}

void Heater::turnHeatElementOnOff(boolean on) {
  digitalWrite(gpioPin, on); //turn pin high
  heaterState = on;
}

// End Heater Control