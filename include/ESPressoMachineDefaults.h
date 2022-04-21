//////
// Default settings
//
//
//////
#ifndef ESPressoMachine_DEFAULTS_H
#define ESPressoMachine_DEFAULTS_H
// Default PID settings
//above target temp
#define S_P 50
#define S_I 0.5
#define S_D 500

// below target temp

#define S_aP 8
#define S_aI 0.1
#define S_aD 400

#define S_TSET 98.5
#define S_TBAND 1.5
#define EQUILIBRIUM_POWER 32.5 // this is the power at which temperature can be maintained at target. Rough estimate will do.
#define MAX_POWER 500  // this is the maximum power when the temperature is above COLDSTART (see below) and still more
                      // than S_TBAND below the S_TSET  target temperature 
#define FAILSAFE_TEMP 112.5 // turn heater off above this temp  should the PID run away.
//
// Intervals for I/O
//
#define HEATER_INTERVAL 100
#define DISPLAY_INTERVAL 1500
#define PID_INTERVAL 500



// GPIO PINS
#define HEAT_RELAY_PIN 13 // 13 + GND are close on the ESP23 DEV Board

#define SENSOR_MAX_DO 26  
#define SENSOR_MAX_CS 27
#define SENSOR_MAX_CLK 14
// 
// Compile time - not to be user configured.
//
#define COLDSTART_TEMP_OFFSET 20 // Below  (S_TSET-COLDSTART_TEMP_OFFSET) the PID vallue will be set to {P,I,D}={COLDSTART_P,0,0}
#define COLDSTART_P 15   // At 20C that is approx full power
#define COLDSTART_MAX_POWER 1000 // play with these vallues if the coldstart starts to slow or overshoots to easy.

#define MAX_COOL 0.025 // 1.5 deg/min

// Compiler options
// options for special modules


#define ENABLE_HTTP
#define ENABLE_TELNET
#define ENABLE_MQTT
//#define ENABLE_SERIAL





#endif