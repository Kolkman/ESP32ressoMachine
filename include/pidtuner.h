#ifndef ESPressiot_TUNING_H
#define ESPressiot_TUNING_H

extern double aTuneStep, aTuneThres;
extern double AvgUpperT, AvgLowerT;
extern int UpperCnt, LowerCnt;
extern int tune_count;

extern unsigned long tune_time;
extern unsigned long tune_start;
void tuning_on();
void tuning_off(); 
void tuning_loop();
#endif