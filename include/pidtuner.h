#ifndef ESPressoMachine_TUNER_H
#define ESPressoMachine_TUNER_H

#include "config.h"
class ESPressoMachine; // Forward declaration

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
        bool tuningOn;
        void tuning_loop();

private:
        double TuneStep, TuneThres;
        double AvgUpperT, AvgLowerT;
        int UpperCnt, LowerCnt;
        int tune_count;
        long tune_time;
        long tune_start;
        ESPressoMachine *myMachine;
};

#endif