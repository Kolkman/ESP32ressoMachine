
#include "wifiManager.h"
#include "EspressoWebServer.h"
#include <ESPAsync_WiFiManager.h>  


WiFiManager::WiFiManager()
{
    myServer = nullptr;
    filesystem = &LittleFS;
    WM_AP_IPconfig._ap_static_ip = AP_STATIC_IP;
    WM_AP_IPconfig._ap_static_gw = AP_STATIC_GW;
    WM_AP_IPconfig._ap_static_sn = AP_STATIC_SN;

    WM_STA_IPconfig._sta_static_ip = STATION_IP;
    WM_STA_IPconfig._sta_static_gw = GATEWAY_IP;
    WM_STA_IPconfig._sta_static_sn = NETMASK;
#if USE_CONFIGURABLE_DNS
    WM_STA_IPconfig._sta_static_dns1 = DNS_1_IP;
    WM_STA_IPconfig._sta_static_dns2 = DNS_2_IP;
#endif
}

void WiFiManager::setup(EspressoWebServer *server)
{
    myServer = server;

    // set led pin as output
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.println(ESP_ASYNC_WIFIMANAGER_VERSION);
#if defined(ESP_ASYNC_WIFIMANAGER_VERSION_INT)
    if (ESP_ASYNC_WIFIMANAGER_VERSION_INT < ESP_ASYNC_WIFIMANAGER_VERSION_MIN)
    {
        Serial.print("Warning. Must use this example on Version later than : ");
        Serial.println(ESP_ASYNC_WIFIMANAGER_VERSION_MIN_TARGET);
    }
#endif
    // Assuming that a LittleFS filesystem has been primed and set up.
    if (!FileFS.begin())
    {
        // prevents debug info from the library to hide err message.
        delay(100);

        Serial.println(F("LittleFS failed!. Please use SPIFFS or EEPROM. Stay forever"));

        while (true)
        {
            // Hang around infinitly...
            delay(1);
        }
    }
    unsigned long startedAt = millis();
    digitalWrite(LED_BUILTIN, LED_ON); // turn the LED on by making the voltage LOW to tell us we are in configuration mode.
//#if (USING_ESP32_S2 || USING_ESP32_C3)
    ESPAsync_WiFiManager ESPAsync_wifiManager((AsyncWebServer *) server, NULL, "AsyncConfigOnSwitch");
//#else
//    DNSServer dnsServer;
 //   ESPAsync_WiFiManager ESPAsync_wifiManager(server, &dnsServer, "AsyncConfigOnSwitch");
//#endif

    ESPAsync_wifiManager.setDebugOutput(true);

    /// REMOVE WHEN DONE
    while (true)
    {
        // Hang around infinitly...
        delay(1);
    }
}

///////////////////////////////////////////////////////////
