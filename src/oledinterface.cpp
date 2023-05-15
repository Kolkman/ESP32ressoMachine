
#include "ESPressoMachine.h"
#ifdef ENABLE_OLED
#include "Oledinterface.h"
#include "debug.h"
#include "math.h"

// Contains DrawGauge based code
// Copyright (c) 2017, Giuseppe Bellomo
//  All rights reserved.
// https://github.com/olikraus/u8g2/blob/master/sys/arduino/u8g2_page_buffer/Gauge/Gauge.ino

void OledInterface::DrawGauge(int x, byte y, byte r, byte p, int v, float targetTemp, float maxOffset)
{
    int ticks = 9;
    int n = (r / 100.00) * p; // calculate needle percent lenght
    float minOfScale = targetTemp - maxOffset;
    float maxOfScale = targetTemp + maxOffset;
    float gs_rad = -PI / 2;
    float ge_rad = PI / 2;
    bool farFromTargetTemp = false;
    if ((v < minOfScale) || (v > maxOfScale))
        farFromTargetTemp = true;

    if (farFromTargetTemp)
    {
        minOfScale = 10;
        maxOfScale = 130;
    }

    float angle = -PI / 2 + (v - minOfScale) * PI / (maxOfScale - minOfScale);
    int xp = x + (sin(angle) * n);
    int yp = y - (cos(angle) * n);
    oled->drawCircle(x, y, r, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
    oled->drawLine(x, y, xp, yp);

    if (farFromTargetTemp)
    {
        LOGINFO3("MinOfScale", minOfScale, "MaxScale", maxOfScale);
        float ticklengthfraction;
        float ticks[] = {minOfScale,
                         targetTemp,
                         100,
                         maxOfScale};
        for (float tick : ticks)
        {
            float angle = -PI / 2 + (tick - minOfScale) * PI / (maxOfScale - minOfScale);
            if (tick == targetTemp)
            {
                ticklengthfraction = 0.6;
            }
            else
            {
                ticklengthfraction = 0.8;
            }
            int tickXstart = x + (sin(angle) * r * ticklengthfraction);
            int tickYstart = y - (cos(angle) * r * ticklengthfraction);
            int tickXend = x + (sin(angle) * r * 1);
            int tickYend = y - (cos(angle) * r * 1);
            oled->drawLine(tickXstart, tickYstart, tickXend, tickYend);
        }

        oled->setFont(u8g2_font_t0_11_tf);
        float xOffset;
        //            float xOffset = oled->getStrWidth(String(targetTemp, 1).c_str()) / 2;
        //            oled->drawStr(x - xOffset, y - r * 1.1, String(targetTemp, 1).c_str());
        float angle = -PI / 2 + (100 - minOfScale) * PI / (maxOfScale - minOfScale);

        oled->drawStr(x + sin(angle) * r * 1.1, y - cos(angle) * r * 1.1, "100");

        xOffset = oled->getStrWidth(String(minOfScale, 0).c_str());
        oled->drawStr(x - r * 1.1 - xOffset, y, String(minOfScale, 0).c_str());
        oled->drawStr(x + r * 1.1, y, String(maxOfScale, 0).c_str());
        oled->drawStr(4, 10, String(targetTemp, 1).c_str());

        /*    xOffset = oled->getStrWidth(String(maxOfScale, 0).c_str());
            oled->drawStr(x + sin(PI / 4) * r * 1.1, y - cos(-PI / 4) * r * 1.1, String(maxOffset / 2, 1).c_str());
*/
    }
    else

    {
        for (int j = 0; j < ticks; j++)
        {
            float angle = -PI / 2 + j * PI / (ticks - 1);
            float tickstart;
            if (j % 2 == 0)
            {
                tickstart = 0.75;
            }
            else
            {
                tickstart = 0.9;
            }
            int tickXstart = x + (sin(angle) * r * tickstart);
            int tickYstart = y - (cos(angle) * r * tickstart);
            int tickXend = x + (sin(angle) * r * 1);
            int tickYend = y - (cos(angle) * r * 1);
            oled->drawLine(tickXstart, tickYstart, tickXend, tickYend);

            oled->setFont(u8g2_font_t0_11_tf);

            float xOffset = oled->getStrWidth(String(targetTemp, 1).c_str()) / 2;
            oled->drawStr(x - xOffset, y - r * 1.1, String(targetTemp, 1).c_str());
            xOffset = oled->getStrWidth(String(-maxOffset, 1).c_str());
            oled->drawStr(x - r * 1.1 - xOffset, y, String(-maxOffset, 1).c_str());
            oled->drawStr(x + r * 1.1, y, String(maxOffset, 1).c_str());
            xOffset = oled->getStrWidth(String(-maxOffset / 2, 1).c_str());
            oled->drawStr(x + sin(-PI / 4) * r * 1.1 - xOffset, y - cos(-PI / 4) * r * 1.1, String(-maxOffset / 2, 1).c_str());
            oled->drawStr(x + sin(PI / 4) * r * 1.1, y - cos(-PI / 4) * r * 1.1, String(maxOffset / 2, 1).c_str());
        }
    }
}

OledInterface::OledInterface()
{
#if defined(OLED_SSD1306_I2C)
    oled = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0);
#elif defined(OLED_SH1106_I2C)
    oled = new U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0);
#else
#error "No proper OLED display defined"
#endif
}

void OledInterface::setupOled()
{
    time_now = millis();
    LOGINFO("SETTING UP INTERFACE");
    oled->begin();
    oled->clearBuffer();
    oled->setFont(u8g2_font_ncenB14_tr);
    oled->drawStr(0, 20, "ESP32resso");
    oled->drawStr(0, 45, FIRMWAREVERSION);
    oled->sendBuffer();
    return;
}

void OledInterface::loopOled(ESPressoMachine *myMachine)
{

    time_now = millis();
    // only update screen every so often
    // example values for testing, use the values you wish to pass as argument while calling the function
    byte cx = 64;      // x center
    byte cy = 44;      // y center
    byte radius = 30;  // radius
    byte percent = 80; // needle percent

    if ((max(time_now, lastUpdateTime) - min(time_now, lastUpdateTime)) >= 500)
    {
        if (myMachine->powerOffMode)
        {
            oled->clearBuffer();

            String tmpOutput = String(myMachine->inputTemp, 1);
            int xOffset = oled->getStrWidth(tmpOutput.c_str()) / 2;
            oled->drawStr(64 - xOffset, 20, tmpOutput.c_str());
            tmpOutput = String("Off");
            xOffset = oled->getStrWidth(tmpOutput.c_str()) / 2;
            oled->drawStr(64 - xOffset, 60, tmpOutput.c_str());

            oled->sendBuffer();
        }
        else
        {

            oled->clearBuffer();
            DrawGauge(cx, cy, radius, percent, (int)myMachine->inputTemp, myMachine->myConfig->targetTemp, 7.5);
            oled->setFont(u8g2_font_ncenB10_tr);
            //        String TempReport = String(myMachine->inputTemp, 1) + " / " + String(myMachine->myConfig->targetTemp, 1);
            String TempReport = String(myMachine->inputTemp, 1);
            int xOffset = oled->getStrWidth(TempReport.c_str()) / 2;
            oled->drawStr(64 - xOffset, 60, TempReport.c_str());
            u8g2_uint_t pwrX = (u8g2_uint_t)(log10(myMachine->outputPwr) / 3 * oled->getDisplayWidth());
            LOGINFO1("pwrX: ", pwrX);

            oled->drawBox(0, 62, pwrX, 3);

            oled->sendBuffer();
        }
    }
}

#endif // ENABLE_OLED
