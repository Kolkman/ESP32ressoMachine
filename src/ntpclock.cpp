#include "ESPressoMachineDefaults.h"

#ifdef ENABLE_NTPCLOCK
#include "ntpClock.h"

#include "debug.h"

ntpClock::ntpClock()
{
    ntpUDP = nullptr;
    timeClient = nullptr;
    currentTimeZone = nullptr;
    initialized = false;
    lastNTPUpdate = 0;
}
/**
 * Input time in epoch format and return tm time format
 * by Renzo Mischianti <www.mischianti.org>
 */
tm ntpClock::getDateTimeByParams(long time)
{
    struct tm *newtime;
    const time_t tim = time;
    newtime = localtime(&tim);
    return *newtime;
}

/**
 * Input tm time format and return String with format pattern
 * by Renzo Mischianti <www.mischianti.org>
 */
String ntpClock::getDateTimeStringByParams(tm *newtime, char *pattern = (char *)"%d/%m/%Y %H:%M:%S")
{
    char buffer[30];
    strftime(buffer, 30, pattern, newtime);
    return buffer;
}

/**
 * Input time in epoch format format and return String with format pattern
 * by Renzo Mischianti <www.mischianti.org>
 */
String ntpClock::getEpochStringByParams(long time, char *pattern = (char *)"%d/%m/%Y %H:%M:%S")
{
    //    struct tm *newtime;
    tm newtime;
    newtime = getDateTimeByParams(time);
    return getDateTimeStringByParams(&newtime, pattern);
}

void ntpClock::setup()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        LOGINFO("Skipping NTP setup: WiFi not connected");
        initialized = false;
        return;
    }

    ntpUDP = new WiFiUDP();
    timeClient = new NTPClient(*ntpUDP, ntpServerName.c_str(), (int)GMT_OFFSET * 60 * 60, 60 * 60 * 1000);
    currentTimeZone = new Timezone(CEST, CET);

    timeClient->begin();
    delay(1000);
    bool timeUpdatePass = true;
    for (int n = 0; n < 20; n++)
    {
        if (timeClient->update())
        {
            timeUpdatePass = true;
            lastNTPUpdate = millis();

            break;
        }
        delay(20);
    }

    if (timeUpdatePass)
    {
        LOGINFO("Adjust local clock");
        unsigned long epoch = timeClient->getEpochTime();
        setTime(epoch);
        initialized = true;
        LOGINFO1("Time initalized to: ", getEpochStringByParams(currentTimeZone->toLocal(now()), (char *)"%H:%M"));
    }
    else
    {
        initialized = false;
        LOGINFO("NTP Update Failed!!");
    }
}
void ntpClock::loop()
{
    if (!initialized || timeClient == nullptr)
    {
        return;
    }

    unsigned long time_now = millis();

    if ((max(time_now, lastNTPUpdate) - min(time_now, lastNTPUpdate)) >= (1000 * CLOCKUPDATE))
    {
   
        for (int n = 0; n < 20; n++)
        {
            if (timeClient->forceUpdate())
            {
                lastNTPUpdate = millis();
                LOGINFO("TIME UPDATED")
                break;
            }
            delayMicroseconds(200);
        }
    }
}

bool ntpClock::hasTime()
{
    return initialized && currentTimeZone != nullptr;
}

String ntpClock::getTimeString()
{
    if (!hasTime())
    {
        return "--:--";
    }

    String timeString = getEpochStringByParams(currentTimeZone->toLocal(now()), (char *)"%H:%M");
    return timeString;
}

#endif // ENABLE_NTPCLOCK