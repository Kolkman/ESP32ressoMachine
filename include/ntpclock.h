
#pragma once
#ifndef ntpClock_h
#define ntpClock_h
#include "ESPressoMachineDefaults.h"
#include "WiFi.h"
#ifdef ENABLE_NTPCLOCK

#define GMT_OFFSET 0
#define UDPPORT 8888
#define NTP_SERVERNAME "nl.pool.ntp.org"
#define DAYLIGHTSAFE_TIMEZONEPARAMS    \
    {                                  \
        "CEST", Last, Sun, Mar, 2, 120 \
    }
#define TIMEZONEPARAMS                \
    {                                 \
        "CET ", Last, Sun, Oct, 3, 60 \
    }
#define CLOCKUPDATE 1800 // update the clock agains NTP only every so often (seconds)
#include <NTPClient.h>
#include <time.h>
#include <Timezone.h>

class ntpClock
{
public:
    ntpClock();
    void setup();
    void loop();
    String getTimeString();

private:
    WiFiUDP * ntpUDP;
    NTPClient * timeClient;
    String ntpServerName = NTP_SERVERNAME;
    TimeChangeRule CEST = DAYLIGHTSAFE_TIMEZONEPARAMS; // Central European Summer Time
    TimeChangeRule CET = TIMEZONEPARAMS;               // Central European Standard Time
    Timezone *currentTimeZone;
    // Central European Time (Frankfurt, Paris)
    unsigned long lastNTPUpdate;
    unsigned int localPort = UDPPORT; // local port to listen for UDP packets
    const int timeZone = 1;           // Central European Time


    tm getDateTimeByParams(long);
    String getDateTimeStringByParams(tm *, char *);
    String getEpochStringByParams(long, char *);
};

#endif // ENABLE_NTPCLOCK
#endif // ntpClock_h