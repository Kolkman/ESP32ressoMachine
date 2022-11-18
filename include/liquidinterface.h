#pragma once
#ifndef LiquidInterface_h
#define LiquidInterface_h

#include "ESPressoMachineDefaults.h"
#ifdef ENABLE_LIQUID
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "ESPressoMachine.h"

#ifndef LCD_ROWS
#define LCD_ROWS 2
#endif

#ifndef LCD_COLUMS 
#define LCD_COLUMS 16
#endif

#define LCD_PAGE   ((LCD_COLUMS) * (LCD_ROWS))
class LiquidInterface
{
public:
    LiquidInterface();
    LiquidCrystal_I2C *lcd;
    void setupLiquid();
    void loopLiquid(ESPressoMachine *);
private:
    bool backlightIsOn;
};

#endif
#endif