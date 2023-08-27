#include "ESPressoMachine.h"
#ifdef ENABLE_LIQUID
#include "liquidinterface.h"
#include "debug.h"
#include "math.h"

LiquidInterface::LiquidInterface()
{
    lcd = new LiquidCrystal_I2C(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
    backlightIsOn = false;
}

void LiquidInterface::setupLiquid()
{
    time_now = millis();
    LOGINFO("SETTING UP INTERFACE");
    while (lcd->begin(LCD_COLUMS, LCD_ROWS) != 1)
    {
        LOGERROR(F("PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
        delay(5000);
    }

    lcd->clear();
    lcd->backlight();
    backlightIsOn = true;
    lcd->setCursor(3, 0);
    lcd->print("ESP32resso");
    lcd->setCursor(2, 1);
    lcd->print(FIRMWAREVERSION);
    return;
}

void LiquidInterface::loopLiquid(ESPressoMachine *myMachine)
{

    time_now = millis();
    // only update screen every so often

    if ((max(time_now, lastUpdateTime) - min(time_now, lastUpdateTime)) >= 500)
    {
        if (myMachine->powerOffMode && backlightIsOn)
        {
            lcd->noBacklight();
            backlightIsOn = false;
        }
        if (!myMachine->powerOffMode && !backlightIsOn){
             lcd->backlight();
            backlightIsOn = true;
        }
        lcd->clear();
        lcd->setCursor(2, 0);
        lcd->print(String(myMachine->inputTemp, 1) + " / " + String(myMachine->myConfig->targetTemp, 1));
        lcd->setCursor(0, 1);
        char powerstring[17];
        powerstring[0] = '\0';
        int i;
        for (i = 0; i < 16; i++)
        {
            if (myMachine->outputPwr == 0)
            {
                break;
            }

            if (log10(myMachine->outputPwr) / 3 * 16 >= i)
            {
                powerstring[i] = 0xff;
            }
            else
            {
                powerstring[i] = '\0';
            }
        }
        powerstring[i] = '\0';
        lcd->print(powerstring);
    }
}

#endif // ENABLE_LIQUID
