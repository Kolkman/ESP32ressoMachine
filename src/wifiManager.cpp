/// O.M. Kolkman
/// This is code draws heavilly from  https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/examples/Async_ConfigOnStartup/Async_ConfigOnStartup.ino
///
/// A number of functions are verbatim copies and so is some of its structure.
/// I wrote this because I wanted a little different functionality, mainly having configuration available after
/// connecting to MultiWifi.
/// I removed a lot of configuration (this is very ESP32 specific now)
#include "ESPressoMachineDefaults.h"
#include "wifiManager.h"
#include "WiFiMulti.h"
#include "EspressoWebServer.h"
#include "EspressoMachine.h"
#include "config.h"

WiFiManager::WiFiManager(ESPressoInterface *_i)
{
    myInterface = _i;
    dnsServer = new AsyncDNSServer;
}

void WiFiManager::setupWiFiAp(WiFi_AP_IPConfig *WifiApIP)
{
    unsigned long startedAt = millis();

    // Initiation of all.
    ApSSID = "ESP32Mach";
    ApPass = AP_PASSWORD + String(ESP_getChipId(), HEX); // TODO make configurable
    String iHostname = "ESP32Mach" + String(ESP_getChipId(), HEX);
    getRFC952_hostname(iHostname.c_str()); // Sets RFC952_hostname attribute

    LOGERROR1(F("Hostname set to"), RFC952_hostname)
    // Remove this line if you do not want to see WiFi password printed
    LOGINFO1(F("WIFI AP setup SSID"), ApSSID);
    myInterface->report("SSID: " + ApSSID, "PW: " + ApPass);
    // This check is copied from ESPAsync_WifiManager
    // Check cores/esp32/esp_arduino_version.h and cores/esp32/core_version.h
#if (defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2))
    WiFi.setHostname(RFC952_hostname);
#else
    // Still have bug in ESP32_S2 for old core. If using WiFi.setHostname() => WiFi.localIP() always = 255.255.255.255
    if (String(ARDUINO_BOARD) != "ESP32S2_DEV")
    {
        // See https://github.com/espressif/arduino-esp32/issues/2537
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.setHostname(RFC952_hostname);
    }
#endif

    // Check if there is stored WiFi router/password credentials.
    // If not found, device will remain in configuration mode until switched off via webserver.
    LOGERROR(F("Starting Setup of AP"));
    _configPortalStart = millis();

    if (WiFi.getAutoConnect() == 0)
        WiFi.setAutoConnect(1);

    // Random Channel Assignment
    static int channel = 10; // random(1, MAX_WIFI_CHANNEL);
    if (!WiFi.softAPConfig(WifiApIP->_ap_static_ip, WifiApIP->_ap_static_gw, WifiApIP->_ap_static_sn))
    {
        LOGINFO(F("Failed setting AP IP address from config"));
    }
    else
    {
        LOGINFO1(F("Configured APP to use"), WifiApIP->_ap_static_ip.toString());
    }

    // Strating the Access Point
    WiFi.softAP(ApSSID.c_str()); 
    delay(100);

    // Future configurable AP IP stuff could go here.

    /* Setup the DNS server redirecting all the domains to the apIP */
    if (dnsServer)
    {
#ifdef USE_ASYNC_DNS
        dnsServer->setErrorReplyCode(AsyncDNSReplyCode::NoError);
#else
        dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
#endif
        // AsyncDNSServer started with "*" domain name, all DNS requests will be passsed to WiFi.softAPIP()
        if (!dnsServer->start(DNS_PORT, "*", WifiApIP->_ap_static_ip))
        {
            // No socket available
            LOGERROR(F("Can't start DNS Server. No available socket"));
        }
        else
            LOGINFO1("DNS for Captive Portal at", WiFi.softAPIP());
    }
    else
    {
        LOGWARN(F("No DNS Server Object Available"));
    }
    LOGWARN1(F("AP IP address ="), WiFi.softAPIP());
    return;
}

void WiFiManager::loopPortal()
{
    connect = false;
    bool TimedOut = true;

    { // Start an asynchronous scan, we are bound to need it soon.
        int n = WiFi.scanComplete();
        if (n == -2)
        {
            WiFi.scanNetworks(true);
        }
    }

    LOGINFO("startConfigPortal : Enter loop");

    while (myInterface->_waitingForClientAction || millis() < _configPortalStart + CONFIGPORTAL_TIMEOUT)
    {
#ifdef USE_ASYNC_DNS
        // left blank
#else
        dnsServer->processNextRequest();
#endif
        yield();
    }
    LOGINFO1(F("Waiting for Client Action"), myInterface->_waitingForClientAction);
    LOGINFO("startConfigPortal : Exits loop");
    return;
}

uint8_t WiFiManager::connectMultiWiFi(EspressoConfig *myConfig)
{

    uint8_t status;
#ifdef HOSTNAME
    getRFC952_hostname(HOSTNAME);

#if (defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 2))
    WiFi.setHostname(RFC952_hostname);
#else
    // Still have bug in ESP32_S2 for old core. If using WiFi.setHostname() => WiFi.localIP() always = 255.255.255.255
    if (String(ARDUINO_BOARD) != "ESP32S2_DEV")
    {
        // See https://github.com/espressif/arduino-esp32/issues/2537
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.setHostname(RFC952_hostname);
    }
#endif

#endif
    LOGERROR(F("ConnectMultiWiFi with :"));
    bool MultiWifiEntrySet = false;
    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
        // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
        if ((String(myConfig->WM_config.WiFi_Creds[i].wifi_ssid) != "") &&
            (strlen(myConfig->WM_config.WiFi_Creds[i].wifi_pw) == 0 || strlen(myConfig->WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE))
        {
            LOGERROR3(F("* Additional SSID = "), myConfig->WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), myConfig->WM_config.WiFi_Creds[i].wifi_pw);
            addAP(myConfig->WM_config.WiFi_Creds[i].wifi_ssid, myConfig->WM_config.WiFi_Creds[i].wifi_pw);
            MultiWifiEntrySet = true;
        }
    }
    if (!MultiWifiEntrySet)
    {
        LOGINFO("Could not set any MultiWiFi networks... Restarting");
        ESP.restart();
    }

    LOGERROR(F("Connecting MultiWifi..."));
    myInterface->report(" Connecting to", "      WIFI");
    /*
        #if !USE_DHCP_IP
            // New in v1.4.0
            configWiFi(WM_STA_IPconfig);
            //////
        #endif
    */
    int i = 0;
    WiFi.mode(WIFI_STA);

    status = run();

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
        LOGERROR2(F("WiFi connected after : "), i, F("trials."));
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

char *WiFiManager::getRFC952_hostname(const char *iHostname)
// From https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/src/ESPAsync_WiFiManager-Impl.h
// (c) by Khoih-prog
{

    memset(RFC952_hostname, 0, sizeof(RFC952_hostname));

    size_t len = (RFC952_HOSTNAME_MAXLEN < strlen(iHostname)) ? RFC952_HOSTNAME_MAXLEN : strlen(iHostname);

    size_t j = 0;

    for (size_t i = 0; i < len - 1; i++)
    {
        if (isalnum(iHostname[i]) || iHostname[i] == '-')
        {
            RFC952_hostname[j] = iHostname[i];
            j++;
        }
    }
    // no '-' as last char
    if (isalnum(iHostname[len - 1]) || (iHostname[len - 1] != '-'))
        RFC952_hostname[j] = iHostname[len - 1];

    return RFC952_hostname;
}