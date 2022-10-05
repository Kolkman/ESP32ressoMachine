/// O.M. Kolkman
/// This is code draws heavilly from  https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/examples/Async_ConfigOnStartup/Async_ConfigOnStartup.ino
///
/// A number of functions are verbatim copies and so is some of its structure.
/// I wrote this because I wanted a little different functionality, mainly having configuration available after
/// connecting to MultiWifi.
/// I removed a lot of configuration (this is very ESP32 specific now)

#include "wifiManager.h"
#include "WiFiMulti.h"
#include "EspressoWebServer.h"
#include "EspressoMachine.h"
#include "config.h"

WiFiManager::WiFiManager()
{
    myInterface = nullptr;
    // myConfig = config;
    dnsServer = new AsyncDNSServer;
    //   wifiMulti = new WiFiMulti;
}

void WiFiManager::setupWiFiAp()
{

    unsigned long startedAt = millis();

    // Initiation of all.
    ApSSID = "ESP32Mach";
    ApPass = AP_PASSWORD + String(ESP_getChipId(), HEX); // TODO make configurable
    String iHostname = "ESP32Mach" + String(ESP_getChipId(), HEX);
    getRFC952_hostname(iHostname.c_str()); // Sets RFC952_hostname attribute

    LOGERROR1(F("Hostname set to"), RFC952_hostname)
    // Remove this line if you do not want to see WiFi password printed
    LOGERROR3(F("WIFI AP setup SSID/PASSWORD"), ApSSID, F("/"), ApPass);
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
    static int channel = random(1, MAX_WIFI_CHANNEL);

    // Strating the Access Point
    WiFi.softAP(ApSSID.c_str(), ApPass.c_str(), channel);
    delay(500);
    // initiate a dnsServer
    if (!dnsServer)
        dnsServer = new AsyncDNSServer;

    // Future configurable AP IP stuff could go here.

    /* Setup the DNS server redirecting all the domains to the apIP */
    if (dnsServer)
    {
        dnsServer->setErrorReplyCode(AsyncDNSReplyCode::NoError);

        // AsyncDNSServer started with "*" domain name, all DNS requests will be passsed to WiFi.softAPIP()
        if (!dnsServer->start(DNS_PORT, "*", WiFi.softAPIP()))
        {
            // No socket available
            LOGERROR(F("Can't start DNS Server. No available socket"));
        }
        else
            LOGINFO("DNS for Captive Portal");
    }
    LOGWARN1(F("AP IP address ="), WiFi.softAPIP());
    
    {// Start an asynchronous scan, we are bound to need it soon.
        int n = WiFi.scanComplete();
        if (n == -2)
        {
            WiFi.scanNetworks(true);
        }
        return;
    }
}

void WiFiManager::loopPortal(ESPressoInterface *myInterface)
{
    connect = false;
    bool TimedOut = true;
    LOGINFO("startConfigPortal : Enter loop");
    // TODO: we should be waiting for Action as long as there is no stored credentials.
    while (myInterface->waitingForClientAction || millis() < _configPortalStart + CONFIGPORTAL_TIMEOUT)
    {
        delay(1);
    }
    LOGINFO1(F("Waiting for Client Action"), myInterface->waitingForClientAction);
    LOGINFO("startConfigPortal : Exits loop");
    return;
}

uint8_t WiFiManager::connectMultiWiFi(EspressoConfig *myConfig)
{

    uint8_t status;

    LOGERROR(F("ConnectMultiWiFi with :"));

    for (uint8_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
    {
        // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
        if ((String(myConfig->WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(myConfig->WM_config.WiFi_Creds[i].wifi_pw) >= MIN_AP_PASSWORD_SIZE))
        {
            LOGERROR3(F("* Additional SSID = "), myConfig->WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), myConfig->WM_config.WiFi_Creds[i].wifi_pw);

            addAP(myConfig->WM_config.WiFi_Creds[i].wifi_ssid, myConfig->WM_config.WiFi_Creds[i].wifi_pw);
        }
    }

    LOGERROR(F("Connecting MultiWifi..."));
    /*
        #if !USE_DHCP_IP
            // New in v1.4.0
            configWiFi(WM_STA_IPconfig);
            //////
        #endif
    */
    int i = 0;
    status = run();
    WiFi.mode(WIFI_STA);

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