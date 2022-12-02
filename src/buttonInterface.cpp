
#include "ESPressoMachine.h"
#ifdef ENABLE_BUTTON
#include "buttonInterface.h"
#include "liquidinterface.h"
#include "debug.h"
#ifndef ENABLE_LIQUID
#error "ENABLE_BUTTON is defined without ENABLE_LIQUID being defined. The BUtton interface needs the LCD"
#endif

ButtonInterface::ButtonInterface()
{
    BlueStartPress = 0;
    RedStartPress = 0;
    BlackStartPress = 0;
    BlueLastPress = 0;
    RedLastPress = 0;
    BlackLastPress = 0;
}

void ButtonInterface::setupButton()
{
    LOGINFO("SETTING UP BUTTON INTERFACE");
    pinMode(BLACK_BUTTON, INPUT_PULLUP); // config GIOP21 as input pin and enable the internal pull-up resistor
    pinMode(BLUE_BUTTON, INPUT_PULLUP);  // config GIOP21 as input pin and enable the internal pull-up resistor
    pinMode(RED_BUTTON, INPUT_PULLUP);   // config GIOP21 as input pin and enable the internal pull-up resistor
}

void ButtonInterface::loopButton(ESPressoMachine *myMachine)
{
    unsigned long now = millis();
    // read the state of the switch/button:
    bool currentBlueState = digitalRead(BLUE_BUTTON);
    bool currentRedState = digitalRead(RED_BUTTON);
    bool currentBlackState = digitalRead(BLACK_BUTTON);
    bool changeToBeReported = false;
    if (BlueStartPress && currentBlueState == HIGH)
    {
        LOGINFO("The blue state changed from LOW to HIGH");
        BlueStartPress = 0;
    }
    if (RedStartPress && currentRedState == HIGH)
    {
        LOGINFO("The red state changed from LOW to HIGH");
        RedStartPress = 0;
    }
    if (BlackStartPress && currentBlackState == HIGH)
    {
        LOGINFO("The black state changed from LOW to HIGH");
        BlackStartPress = 0;
    }

    if (!BlueStartPress && currentBlueState == LOW)
    {

        LOGINFO("The blue state changed from HIGH to LOW");
        BlueStartPress = now;
        BlueLastPress = BlueStartPress;
        myMachine->myConfig->targetTemp += -0.1;
        changeToBeReported = true;
    }
    if (!RedStartPress == HIGH && currentRedState == LOW)
    {
        LOGINFO("The red state changed from HIGH to LOW");
        RedStartPress = now;
        RedLastPress = RedStartPress;
        myMachine->myConfig->targetTemp += 0.1;
        changeToBeReported = true;
    }

    if (!BlackStartPress == HIGH && currentBlackState == LOW)
    {
        LOGINFO("The black state changed from HIGH to LOW");
        BlackStartPress = now;
        BlackLastPress = BlackStartPress;
    }

    if (BlueStartPress && currentBlueState == LOW)
    {

        if ((now - BlueLastPress) > SINGLEPRESS_T)
        {
            LOGINFO1("The blue: ", now - BlueStartPress);
            BlueLastPress = now;
            if ((now - BlueStartPress) > LONGPRESS_T)
                myMachine->myConfig->targetTemp += -1;
            else
                myMachine->myConfig->targetTemp += -0.1;
            changeToBeReported = true;
        }
    }
    if (RedStartPress && currentRedState == LOW)
    {

        if ((now - RedLastPress) > SINGLEPRESS_T)
        {
            LOGINFO1("The red state: ", now - RedStartPress);
            RedLastPress = now;
            if ((now - RedStartPress) > LONGPRESS_T)
                myMachine->myConfig->targetTemp += 1;
            else
                myMachine->myConfig->targetTemp += 0.1;
            changeToBeReported = true;
        }
    }
    if (BlackStartPress && currentBlackState == LOW)
    {

        if ((now - BlackLastPress) > SINGLEPRESS_T)
        {
            LOGINFO1("The black: ", now - BlackStartPress);
            BlackLastPress = now;
            if ((now - BlackStartPress) > LONGPRESS_T)
            {
                if (!myMachine->powerOffMode)
                {
                    myMachine->powerOffMode = true;
                    String filler = ">";
                    for (int i; i < 16; i++)
                    {
                        myMachine->myInterface->report("Turning PID contol off", filler);
                        delay(100);
                        filler = "-" + filler;
                    }
                    // Give some time to let loose of the button.
                    BlackStartPress=millis();
                    BlackLastPress = now;
                }
                else
                {
                    myMachine->powerOffMode = false;
                     String filler = ">";
                    for (int i; i < 16; i++)
                    {
                        myMachine->myInterface->report("Starting PID contol", filler);
                        delay(100);
                        filler = "+" + filler;
                    }
                               // Give some time to let loose of the button.
                    BlackStartPress=millis();
                    BlackLastPress = now;
                    changeToBeReported = true;
                }
            }
        }
    }
    if (changeToBeReported)
    {

        myMachine->myInterface->loopLiquid(myMachine);
    }
}

#endif // ENABLE_Button
