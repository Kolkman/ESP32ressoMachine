//
// ESPressIoT Controller for Espresso Machines
// 2016-2021 by Roman Schmitz
//
// Thermostat Switch Detection - basically reading the state of a digital GPIO used to detect the state of a a switch/button together with a pull-up/down resistor (here: 100k to GND)
//

#define SWITCH_GPIO D7
#define SWITCH_SMP_TIME 100

#ifdef ENABLE_SWITCH_DETECTION

unsigned long lastSwitchTime;

int gButtonState = 0; 

void setupSwitch() {
  pinMode(SWITCH_GPIO, INPUT);
}

void loopSwitch() {
  if (abs(millis() - lastSwitchTime) >= SWITCH_SMP_TIME) {
    gButtonState = digitalRead(SWITCH_GPIO);
  }
}
#endif