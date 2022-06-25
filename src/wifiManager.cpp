///
/// This is following some hints from https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/examples/Async_ConfigOnStartup/Async_ConfigOnStartup.ino
/// We want the wifimanager to connect to preconfigured network after 60 seconds.
///

#include "wifiManager.h"
#include "EspressoWebServer.h"
#include <ESPAsync_WiFiManager.h>

WiFiManager::WiFiManager()
{
    myServer = nullptr;
    filesystem = &LittleFS;
    dnsServer = new DNSServer;
    wifiMulti = new WiFiMulti;
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
                                       // ESPAsync_WiFiManager ESPAsync_wifiManager((AsyncWebServer *)server, NULL, "AsyncConfigOnSwitch");
    //#else
    if (!myServer){
        Serial.print("WARNING SERVER NULLPTR");
        while (true)
        {
            // Hang around infinitly...
            delay(1);
        }
    }
    ESPAsync_WiFiManager ESPAsync_wifiManager((AsyncWebServer *)myServer, dnsServer, "ESP32ressoMachine");
    //#endif

    ESPAsync_wifiManager.setDebugOutput(true);

#if USE_CUSTOM_AP_IP
    // set custom ip for portal
    //  New in v1.4.0
    ESPAsync_wifiManager.setAPStaticIPConfig(WM_AP_IPconfig);
    //////
#endif
    ESPAsync_wifiManager.setMinimumSignalQuality(-1);

    // From v1.0.10 only
    // Set config portal channel, default = 1. Use 0 => random channel from 1-13
    ESPAsync_wifiManager.setConfigPortalChannel(0);
    //////

#if !USE_DHCP_IP
    // Set (static IP, Gateway, Subnetmask, DNS1 and DNS2) or (IP, Gateway, Subnetmask). New in v1.0.5
    // New in v1.4.0
    ESPAsync_wifiManager.setSTAStaticIPConfig(WM_STA_IPconfig);
    //////
#endif

#if USING_CORS_FEATURE // leads to compile time errors.
    ESPAsync_wifiManager.setCORSHeader("Your Access-Control-Allow-Origin");
#endif

    // We can't use WiFi.SSID() in ESP32 as it's only valid after connected.
    // SSID and Password stored in ESP32 wifi_ap_record_t and wifi_config_t are also cleared in reboot
    // Have to create a new function to store in EEPROM/SPIFFS for this purpose
    Router_SSID = ESPAsync_wifiManager.WiFi_SSID();
    Router_Pass = ESPAsync_wifiManager.WiFi_Pass();

    // Remove this line if you do not want to see WiFi password printed
    Serial.println("ESP Self-Stored: SSID = " + Router_SSID + ", Pass = " + Router_Pass);

    // Check if there is stored WiFi router/password credentials.
    // If not found, device will remain in configuration mode until switched off via webserver.
    Serial.println(F("Opening configuration portal."));

    bool configDataLoaded = false;

    // From v1.1.0, Don't permit NULL password
    if ((Router_SSID != "") && (Router_Pass != ""))
    {
        LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass);
        wifiMulti->addAP(Router_SSID.c_str(), Router_Pass.c_str());
        ESPAsync_wifiManager.setConfigPortalTimeout(120); // If no access point name has been previously entered disable timeout.
        Serial.println(F("Got ESP Self-Stored Credentials. Timeout 120s for Config Portal"));
    }

    if (loadConfigData())
    {
        configDataLoaded = true;

        ESPAsync_wifiManager.setConfigPortalTimeout(120); // If no access point name has been previously entered disable timeout.
        Serial.println(F("Got stored Credentials. Timeout 120s for Config Portal"));

#if USE_ESP_WIFIMANAGER_NTP
        if (strlen(WM_config.TZ_Name) > 0)
        {
            LOGERROR3(F("Current TZ_Name ="), WM_config.TZ_Name, F(", TZ = "), WM_config.TZ);
            // configTzTime(WM_config.TZ, "pool.ntp.org" );
            configTzTime(WM_config.TZ, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
        }
        else
        {
            Serial.println(F("Current Timezone is not set. Enter Config Portal to set."));
        }
#endif
    }
    else
    {
        // Enter CP only if no stored SSID on flash and file
        Serial.println(F("Open Config Portal without Timeout: No stored Credentials."));
        initialConfig = true;
    }

    // SSID to uppercase
    ssid.toUpperCase();

    Serial.print(F("Starting configuration portal @ "));

#if USE_CUSTOM_AP_IP
    Serial.print(AP_STATIC_IP);
#else
    Serial.print(F("192.168.4.1"));
#endif

    Serial.print(F(", SSID = "));
    Serial.print(ssid);
    Serial.print(F(", PWD = "));
    Serial.println(password);

    // Starts an access point
    if (!ESPAsync_wifiManager.startConfigPortal((const char *)ssid.c_str(), password))

        Serial.println(F("Not connected to WiFi but continuing anyway."));
    else
    {
        Serial.println(F("WiFi connected...yeey :)"));
    }

    /// REMOVE WHEN DONE
    while (true)
    {
        // Hang around infinitly...
        delay(10);
        Serial.print(".");
    }
}

bool WiFiManager::loadConfigData()
{
    File file = FileFS.open(CONFIG_FILENAME, "r");
    LOGERROR(F("LoadWiFiCfgFile "));

    memset((void *)&WM_config, 0, sizeof(WM_config));

    // New in v1.4.0
    memset((void *)&WM_STA_IPconfig, 0, sizeof(WM_STA_IPconfig));
    //////

    if (file)
    {
        file.readBytes((char *)&WM_config, sizeof(WM_config));

        // New in v1.4.0
        file.readBytes((char *)&WM_STA_IPconfig, sizeof(WM_STA_IPconfig));
        //////

        file.close();
        LOGERROR(F("OK"));

        if (WM_config.checksum != calcChecksum((uint8_t *)&WM_config, sizeof(WM_config) - sizeof(WM_config.checksum)))
        {
            LOGERROR(F("WM_config checksum wrong"));

            return false;
        }

        // New in v1.4.0
        displayIPConfigStruct(WM_STA_IPconfig);
        //////

        return true;
    }
    else
    {
        LOGERROR(F("failed"));

        return false;
    }
}

int WiFiManager::calcChecksum(uint8_t *address, uint16_t sizeToCalc)
{
    uint16_t checkSum = 0;

    for (uint16_t index = 0; index < sizeToCalc; index++)
    {
        checkSum += *(((byte *)address) + index);
    }

    return checkSum;
}

void WiFiManager::displayIPConfigStruct(WiFi_STA_IPConfig in_WM_STA_IPconfig)
{
    LOGERROR3(F("stationIP ="), in_WM_STA_IPconfig._sta_static_ip, ", gatewayIP =", in_WM_STA_IPconfig._sta_static_gw);
    LOGERROR1(F("netMask ="), in_WM_STA_IPconfig._sta_static_sn);
#if USE_CONFIGURABLE_DNS
    LOGERROR3(F("dns1IP ="), in_WM_STA_IPconfig._sta_static_dns1, ", dns2IP =", in_WM_STA_IPconfig._sta_static_dns2);
#endif
}

void WiFiManager::configWiFi(WiFi_STA_IPConfig in_WM_STA_IPconfig)
{
#if USE_CONFIGURABLE_DNS
    // Set static IP, Gateway, Subnetmask, DNS1 and DNS2. New in v1.0.5
    WiFi.config(in_WM_STA_IPconfig._sta_static_ip, in_WM_STA_IPconfig._sta_static_gw, in_WM_STA_IPconfig._sta_static_sn, in_WM_STA_IPconfig._sta_static_dns1, in_WM_STA_IPconfig._sta_static_dns2);
#else
    // Set static IP, Gateway, Subnetmask, Use auto DNS1 and DNS2.
    WiFi.config(in_WM_STA_IPconfig._sta_static_ip, in_WM_STA_IPconfig._sta_static_gw, in_WM_STA_IPconfig._sta_static_sn);
#endif
}

///////////////////////////////////////////////////////////
