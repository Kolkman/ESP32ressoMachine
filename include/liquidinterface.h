#pragma once
#ifndef LiquidInterface_h
#define LiquidInterface_h

#include "ESPressoMachineDefaults.h"
#ifdef ENABLE_LIQUID
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "ESPressoMachine.h"

#ifndef LCD_ADDRESS
#define LCD_ADDRESS 0x27
#endif



class LiquidInterface
{
public:
    LiquidInterface();
    LiquidCrystal_I2C *lcd;
    void setupLiquid();
    void loopLiquid(ESPressoMachine *);
};

#endif
#endif