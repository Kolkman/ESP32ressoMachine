#ifndef ESPressiot_HEATER_H
#define ESPressiot_HEATER_H
#include "ESPressoMachineDefaults.h"

#ifndef HEAT_RELAY_PIN
  #define HEAT_RELAY_PIN 13
#endif

#ifndef HEATER_INTERVAL
    #define HEATER_INTERVAL 1000
#endif


class Heater {
    public:
        Heater(int, unsigned long, bool);  //constructor 

                                //1st argument is the GPIO pin that drives the relais
                                //2nd argument the intervall (ms) by which the heater will be polled (default 1000)
                                //3rd agrument true if in simulation mode (Default false)
 
        Heater();   
        void setHeatPowerPercentage(float);
        float getHeatCycles();
        void updateHeater(unsigned long); // argument: time_now
        void setHeaterInterval(unsigned long);
        unsigned long getHeaterInterval();

    private:
        int gpioPin;    
        float heatCycles;  // the number of millis out of 1000 for the current heat amount (percent * 10)
        bool heaterState;
        bool simmulationMode;
        unsigned long heaterInterval;
        unsigned long heatCurrentTime, heatLastTime;
        void turnHeatElementOnOff(bool);
        void init(int,unsigned long, bool);


};



#endif