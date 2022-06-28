///
/// This is following some hints from https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/examples/Async_ConfigOnStartup/Async_ConfigOnStartup.ino
/// We want the wifimanager to connect to preconfigured network after 120 seconds.
///

#include "wifiManager.h"
#include "WifiSecrets.h"
#include "EspressoWebServer.h"
#include <ESPAsync_WiFiManager.h>

WiFiManager::WiFiManager()
{
    myServer = nullptr;
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
    if (!myServer)
    {
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
    #include "pages/ESPconfig.css.h"  // Produced from WEBsources/ESPconfig.css using the make script....
    ESPAsync_wifiManager.setCustomHeadElement(ESPconfig_css);  
    ESPAsync_WMParameter custom_mqtt_server("server", "mqtt server", "iot.eclipse", 40, " readonly");
    ESPAsync_wifiManager.addParameter(&custom_mqtt_server);


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

        // Only clear then save data if CP entered and with new valid Credentials
        // No CP => stored getSSID() = ""
        if (String(ESPAsync_wifiManager.getSSID(0)) != "" && String(ESPAsync_wifiManager.getSSID(1)) != "")
        {
            // Stored  for later usage, from v1.1.0, but clear first
            memset(&WM_config, 0, sizeof(WM_config));

            for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
            {
                String tempSSID = ESPAsync_wifiManager.getSSID(i);
                String tempPW = ESPAsync_wifiManager.getPW(i);

                if (strlen(tempSSID.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1)
                    strcpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str());
                else
                    strncpy(WM_config.WiFi_Creds[i].wifi_ssid, tempSSID.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_ssid) - 1);

                if (strlen(tempPW.c_str()) < sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1)
                    strcpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str());
                else
                    strncpy(WM_config.WiFi_Creds[i].wifi_pw, tempPW.c_str(), sizeof(WM_config.WiFi_Creds[i].wifi_pw) - 1);

                // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
                if ((String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE))
                {
                    LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw);
                    wifiMulti->addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
                }
            }

#if USE_ESP_WIFIMANAGER_NTP
            String tempTZ = ESPAsync_wifiManager.getTimezoneName();

            if (strlen(tempTZ.c_str()) < sizeof(WM_config.TZ_Name) - 1)
                strcpy(WM_config.TZ_Name, tempTZ.c_str());
            else
                strncpy(WM_config.TZ_Name, tempTZ.c_str(), sizeof(WM_config.TZ_Name) - 1);

            const char *TZ_Result = ESPAsync_wifiManager.getTZ(WM_config.TZ_Name);

            if (strlen(TZ_Result) < sizeof(WM_config.TZ) - 1)
                strcpy(WM_config.TZ, TZ_Result);
            else
                strncpy(WM_config.TZ, TZ_Result, sizeof(WM_config.TZ_Name) - 1);

            if (strlen(WM_config.TZ_Name) > 0)
            {
                LOGERROR3(F("Saving current TZ_Name ="), WM_config.TZ_Name, F(", TZ = "), WM_config.TZ);

#if ESP8266
                configTime(WM_config.TZ, "pool.ntp.org");
#else
                // configTzTime(WM_config.TZ, "pool.ntp.org" );
                configTzTime(WM_config.TZ, "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org");
#endif
            }
            else
            {
                LOGERROR(F("Current Timezone Name is not set. Enter Config Portal to set."));
            }
#endif
            // New in v1.4.0
            ESPAsync_wifiManager.getSTAStaticIPConfig(WM_STA_IPconfig);
            //////

            saveConfigData();

            initialConfig = true;
        }

        digitalWrite(LED_BUILTIN, LED_OFF); // Turn led off as we are not in configuration mode.

        startedAt = millis();

        if (!initialConfig)
        {
            // Load stored data, the addAP ready for MultiWiFi reconnection
            if (!configDataLoaded)
                loadConfigData();

            for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
            {
                // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
                if ((String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE))
                {
                    LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw);
                    wifiMulti->addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
                }
            }

            if (WiFi.status() != WL_CONNECTED)
            {
                Serial.println(F("ConnectMultiWiFi in setup"));

                connectMultiWiFi();
            }
        }

        Serial.print(F("After waiting "));
        Serial.print((float)(millis() - startedAt) / 1000L);
        Serial.print(F(" secs more in setup(), connection result is "));

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.print(F("connected. Local IP: "));
            Serial.println(WiFi.localIP());
        }
        else
            Serial.println(ESPAsync_wifiManager.getStatus(WiFi.status()));

        /// REMOVE WHEN DONE
        /*
        while (true)
        {
            // Hang around infinitly...
            delay(10);
            Serial.print(".");
        }
        */
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

void WiFiManager::saveConfigData()
{
        File file = FileFS.open(CONFIG_FILENAME, "w");
        LOGERROR(F("SaveWiFiCfgFile "));

        if (file)
        {
            WM_config.checksum = calcChecksum((uint8_t *)&WM_config, sizeof(WM_config) - sizeof(WM_config.checksum));

            file.write((uint8_t *)&WM_config, sizeof(WM_config));

            displayIPConfigStruct(WM_STA_IPconfig);

            // New in v1.4.0
            file.write((uint8_t *)&WM_STA_IPconfig, sizeof(WM_STA_IPconfig));
            //////

            file.close();
            LOGERROR(F("OK"));
        }
        else
        {
            LOGERROR(F("failed"));
        }
}
///////////////////////////////////////////////////////////

uint8_t WiFiManager::connectMultiWiFi()
{
        // For ESP32, this better be 0 to shorten the connect time.
        // For ESP32-S2/C3, must be > 500
        uint8_t status;

        LOGERROR(F("ConnectMultiWiFi with :"));

        if ((Router_SSID != "") && (Router_Pass != ""))
        {
            LOGERROR3(F("* Flash-stored Router_SSID = "), Router_SSID, F(", Router_Pass = "), Router_Pass);
            LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass);
            wifiMulti->addAP(Router_SSID.c_str(), Router_Pass.c_str());
        }
        for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
        {
            // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
            if ((String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE))
            {
                LOGERROR3(F("* Additional SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw);
            }
        }

        LOGERROR(F("Connecting MultiWifi..."));

#if !USE_DHCP_IP
        // New in v1.4.0
        configWiFi(WM_STA_IPconfig);
        //////
#endif

        int i = 0;
        status = wifiMulti->run();
        delay(WIFI_MULTI_1ST_CONNECT_WAITING_MS);

        while ((i++ < 20) && (status != WL_CONNECTED))
        {
            status = WiFi.status();

            if (status == WL_CONNECTED)
                break;
            else
                delay(WIFI_MULTI_CONNECT_WAITING_MS);
        }

        if (status == WL_CONNECTED)
        {
            LOGERROR1(F("WiFi connected after time: "), i);
            LOGERROR3(F("SSID:"), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
            LOGERROR3(F("Channel:"), WiFi.channel(), F(",IP address:"), WiFi.localIP());
        }
        else
        {
            LOGERROR(F("WiFi not connected"));

            ESP.restart();
        }

        return status;
}