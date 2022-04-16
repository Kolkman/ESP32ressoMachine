#ifndef ESPressoMachine_TELNET_H
#define ESPressoMachine_TELNET_H

#include <Arduino.h>
#include <WIFI.h>

class TelnetInterface {
public: 
    TelnetInterface();
    void telnetStatus(String);
    void setupTelnet();
    void loopTelnet(String);
    WiFiServer telnetServer;
    WiFiClient telnetClient;
private:


};


#endif