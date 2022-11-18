#include "ESPressoMachine.h"
#ifdef ENABLE_LIQUID
#include "liquidinterface.h"
#include "debug.h"

LiquidInterface::LiquidInterface()
{
    lcd = new LiquidCrystal_I2C(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
    backlightIsOn = false;
}

void LiquidInterface::setupLiquid()
{
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
    if (myMachine->powerOffMode && backlightIsOn)
    {
        lcd->noBacklight();
        backlightIsOn = false;
    }
    lcd->clear();
    lcd->setCursor(6, 0);
    lcd->print(String(myMachine->inputTemp, 1));
}

#endif // ENABLE_LIQUID
