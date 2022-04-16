#ifndef ESPressoMachine_TUNER_H
#define ESPressoMachine_TUNER_H

#include "config.h"
class ESPressoMachine ; // Forward declaration

class PidTuner
{
public:
        PidTuner(ESPressoMachine *);
 
        void setTuneStep(double);
        void setTuneThres(double);
        double getTuneStep();
        double getTuneThres();
        void setTuneCount(int);
        int getTuneCount();
        unsigned long timeElapsed();
        float averagePeriod();
        float upperAverage();
        float lowerAverage();
        float averagePeakToPeak();
        bool tuning_on();
        bool tuning_off();    
        void tuning_loop();
private:



        double TuneStep, TuneThres;
        double AvgUpperT, AvgLowerT;
        int UpperCnt, LowerCnt;
        int tune_count; 
        unsigned long tune_time;
        unsigned long tune_start;
        bool tunerIsOn;
        ESPressoMachine * myMachine;

        
        };

#endif