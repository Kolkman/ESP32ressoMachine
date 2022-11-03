#ifndef ESPRressoMach_MAX31855_H
#define ESPRressoMach_MAX31855_H

#include "ESPressoMachineDefaults.h"
#include <Adafruit_MAX31855.h>


#ifndef SENSOR_MAX_DO
    #define SENSOR_MAX_DO 25
#endif
#ifndef SENSOR_MAX_CS
    #define SENSOR_MAX_CS 26
#endif
#ifndef SENSOR_MAX_CLK
    #define SENSOR_MAX_CLK 27
#endif


#define MAX31855_SMP_TIME 5 // sample time


class TempSensor 
{
public:
    TempSensor();
    void setupSensor();
    void updateTempSensor(double);
    float getTemp(float);
  //  Adafruit_MAX31855 * thermoc;

private:
    double lastT;
    double SumT;
    double lastI;
    double SumI;
    double lastErr;
    int CntT;
    int CntI;
    unsigned long lastSensTime;
    unsigned long time_now;
     Adafruit_MAX31855 * thermocouple;
};

#endif