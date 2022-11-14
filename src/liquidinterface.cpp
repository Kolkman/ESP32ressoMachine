#include "ESPressoMachine.h"
#ifdef ENABLE_LIQUID
#include "liquidinterface.h"
#include "debug.h"


LiquidInterface::LiquidInterface()
{
    lcd= new LiquidCrystal_I2C(LCD_ADDRESS,16,2);
}

void LiquidInterface::setupLiquid(){
    LOGINFO("SETTING UP INTERFACE")
    lcd->init();
    lcd->clear();
    lcd->backlight();
    lcd->setCursor(2,0);
    lcd->print("ESP32resso");
    lcd->setCursor(2,1);
    lcd->print(FIRMWAREVERSION);
    return;
}








#endif



