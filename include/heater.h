#ifndef ESPressiot_HEATER_H
#define ESPressiot_HEATER_H





class HEATER {
    public:
        HEATER(int, unsigned long, bool);  //constructor 

                                //1st argument is the GPIO pin that drives the relais
                                //2nd argument the intervall (ms) by which the heater will be polled (default 1000)
                                //3rd agrument true if in simulation mode (Default false
 
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


};



#endif