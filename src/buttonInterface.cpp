
#include "ESPressoMachine.h"
#ifdef ENABLE_BUTTON
#include "buttonInterface.h"
#include "debug.h"
#if !(defined(ENABLE_LIQUID) || defined(ENABLE_OLED))
#error "ENABLE_BUTTON is defined without ENABLE_LIQUID or ENABLE_OLED being defined. You need an interface to use buttons"
#endif
#ifdef ENABLE_LIQUID
#include "liquidinterface.h"
#endif
#ifdef ENABLE_OLED
#include "oledinterface.h"
#endif

namespace {
bool runBlackButtonProgress(ESPressoMachine *myMachine, const String &message,
                            char fillCharacter) {
  String filler = ">";

  for (int i = 0; i < 16; i++) {
    if (digitalRead(BLACK_BUTTON) != LOW) {
      return false;
    }
    myMachine->myInterface->report(message, filler);
    delay(100);
    if (digitalRead(BLACK_BUTTON) != LOW) {
      return false;
    }
    filler = String(fillCharacter) + filler;
  }

  return digitalRead(BLACK_BUTTON) == LOW;
}

bool showBlackButtonHoldMessage(ESPressoMachine *myMachine,
                                const String &message,
                                const String &detail,
                                unsigned long durationMs) {
  unsigned long startTime = millis();

  while ((millis() - startTime) < durationMs) {
    if (digitalRead(BLACK_BUTTON) != LOW) {
      return false;
    }
    myMachine->myInterface->report(message, detail);
    delay(100);
    if (digitalRead(BLACK_BUTTON) != LOW) {
      return false;
    }
  }

  return digitalRead(BLACK_BUTTON) == LOW;
}
}  // namespace

ButtonInterface::ButtonInterface() {
#ifndef ONLY_BLACK_BUTTON
  BlueStartPress = 0;
  RedStartPress = 0;
  BlueLastPress = 0;
  RedLastPress = 0;
#else
#warning "Only Black Button is used undifine ONLY_BLACK_BUTTON if you want all buttons to work"
#endif
  BlackStartPress = 0;
  BlackLastPress = 0;
}

bool ButtonInterface::setupButton(ESPressoMachine *myMachine) {

  bool initiateConfig = false;  // Config only when the magic button is pressed.
  LOGINFO("SETTING UP BUTTON INTERFACE");
  pinMode(BLACK_BUTTON, INPUT_PULLUP);  // config GIOPXX as input pin and enable
                                        // the internal pull-up resistor
#ifndef ONLY_BLACK_BUTTON
  pinMode(BLUE_BUTTON, INPUT_PULLUP);  // config GIOPYY  as input pin and enable
                                       // the internal pull-up resistor
  pinMode(RED_BUTTON, INPUT_PULLUP);   // config GIOPZZ as input pin and enable
                                       // the internal pull-up resistor
#endif
  LOGINFO1("BLACK_BUTTON", String(BLACK_BUTTON));
#ifndef ONLY_BLACK_BUTTON
  LOGINFO1("BLUE_BUTTON", String(BLUE_BUTTON));
  LOGINFO1("RED_BUTTON", String(RED_BUTTON));
#endif
  myMachine->myInterface->report("Press Black btn", "to enter config");
  // We enter a small loop now, waiting for the black button to be pressed.
  unsigned long startTime = millis();

  while ((millis() - startTime) < 10000) {  // 10 seconds to press the black button
    if (digitalRead(BLACK_BUTTON) == LOW) {
      initiateConfig = true;
      break;
    }
  }

  LOGINFO1("intiateConfig value: ", String(initiateConfig));
  return (initiateConfig);
}

void ButtonInterface::loopButton(ESPressoMachine *myMachine) {
  unsigned long now = millis();
// read the state of the switch/button:
#ifndef ONLY_BLACK_BUTTON
  bool currentBlueState = digitalRead(BLUE_BUTTON);
  bool currentRedState = digitalRead(RED_BUTTON);
#endif
  bool currentBlackState = digitalRead(BLACK_BUTTON);
  bool changeToBeReported = false;
#ifndef ONLY_BLACK_BUTTON
  if (BlueStartPress && currentBlueState == HIGH) {
    LOGINFO("The blue state changed from LOW to HIGH");
    BlueStartPress = 0;
  }
  if (RedStartPress && currentRedState == HIGH) {
    LOGINFO("The red state changed from LOW to HIGH");
    RedStartPress = 0;
  }
#endif
  if (BlackStartPress && currentBlackState == HIGH) {
    LOGINFO("The black state changed from LOW to HIGH");
    BlackStartPress = 0;
    BlackLastPress = 0;
  }

#ifndef ONLY_BLACK_BUTTON
  if (!BlueStartPress && currentBlueState == LOW) {

    LOGINFO("The blue state changed from HIGH to LOW");
    BlueStartPress = now;
    BlueLastPress = BlueStartPress;
    myMachine->myConfig->targetTemp += -0.1;
    changeToBeReported = true;
  }
  if (!RedStartPress == HIGH && currentRedState == LOW) {
    LOGINFO("The red state changed from HIGH to LOW");
    RedStartPress = now;
    RedLastPress = RedStartPress;
    myMachine->myConfig->targetTemp += 0.1;
    changeToBeReported = true;
  }
#endif

  if (!BlackStartPress == HIGH && currentBlackState == LOW) {
    LOGINFO("The black state changed from HIGH to LOW");
    BlackStartPress = now;
    BlackLastPress = BlackStartPress;
  }
#ifndef ONLY_BLACK_BUTTON
  if (BlueStartPress && currentBlueState == LOW) {

    if ((now - BlueLastPress) > SINGLEPRESS_T) {
      LOGINFO1("The blue: ", now - BlueStartPress);
      BlueLastPress = now;
      if ((now - BlueStartPress) > LONGPRESS_T) {
        myMachine->myConfig->targetTemp += -1;
      } else {
        myMachine->myConfig->targetTemp += -0.1;
      }
      changeToBeReported = true;
    }
  }
  if (RedStartPress && currentRedState == LOW) {

    if ((now - RedLastPress) > SINGLEPRESS_T) {
      LOGINFO1("The red state: ", now - RedStartPress);
      RedLastPress = now;
      if ((now - RedStartPress) > LONGPRESS_T) {
        myMachine->myConfig->targetTemp += 1;
      } else {
        myMachine->myConfig->targetTemp += 0.1;
      }
      changeToBeReported = true;
    }
  }
#endif
  if (BlackStartPress && currentBlackState == LOW) {

    if ((now - BlackLastPress) > SINGLEPRESS_T) {
      LOGINFO1("The black: ", now - BlackStartPress);
      BlackLastPress = now;
      if ((now - BlackStartPress) > SINGLEPRESS_T * 3) {
        bool pidToggleCompleted = false;

        if (!myMachine->powerOffMode) {
          LOGINFO("Turning PID OFF")
          pidToggleCompleted =
              runBlackButtonProgress(myMachine, "Turning PID off", '-');
          if (pidToggleCompleted) {
            myMachine->powerOffMode = true;
          }
        } else {
          LOGINFO("Turning PID ON")
          pidToggleCompleted =
              runBlackButtonProgress(myMachine, "Turning PID on", '+');
          if (pidToggleCompleted) {
            myMachine->pwrSafeTimer = millis();
            myMachine->powerOffMode = false;
          }
        }

        changeToBeReported = true;
        if (pidToggleCompleted) {
          String pidStatusMessage = myMachine->powerOffMode ? "Pid off; continue"
                                                            : "Pid on; continue";
          if (showBlackButtonHoldMessage(myMachine, pidStatusMessage,
                                         " press for reboot",
                                         2000) &&
              runBlackButtonProgress(myMachine, "rebooting", '#')) {
            LOGINFO("Rebooting after black-button hold");
            myMachine->myInterface->report("rebooting", "");
            delay(2000);
            ESP.restart();
            return;
          }
        }
      }
    }
  }
  if (changeToBeReported) {
#ifdef ENABLE_LIQUID
    myMachine->myInterface->loopLiquid(myMachine);
#endif
#ifdef ENABLE_OLED
    myMachine->myInterface->loopOled(myMachine);
#endif
  }
}

#endif  // ENABLE_Button
