#pragma once
#ifndef OledInterface_h
#define OledInterface_h

#include "ESPressoMachineDefaults.h"
#ifdef ENABLE_OLED
#include <Arduino.h>
#include <U8g2lib.h>
#include "ESPressoMachine.h"
#include <Wire.h>




class OledInterface
{
public:
    OledInterface();
    #ifdef OLED_SSD1306_I2C
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C * oled;
    #endif
    #ifdef OLED_SH1106_I2C
    U8G2_SH1106_128X64_NONAME_F_HW_I2C * oled;
    #endif
    void setupOled();
    void loopOled(ESPressoMachine *);
    bool oledOperational;

private:
    unsigned long lastUpdateTime;
    unsigned long time_now;
    void DrawGauge(int , byte, byte , byte , int, float, float);
};

#endif
#endif