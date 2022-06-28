#ifndef wifiManager_h
#define wifiManager_h
#include "ESPressoMachineDefaults.h"
#include "EspressoWebServer.h"

#ifndef CONFIG_NETWORK_PASSWORD
CONFIG_NETWORK_PASSWORD "ESP32ressoMachine"
#endif
// Anything Wifi Goes into this class..

// Follows the examples from https://github.com/khoih-prog/ESPAsync_WiFiManager/blob/master/examples/Async_ConfigOnSwitch/Async_ConfigOnSwitch.ino
// and others with a lottle of the cruft cut out. We know we are on an ESP32 and we
// use LittleFS

#if !(defined(ESP32))
#error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif

#define ESP_ASYNC_WIFIMANAGER_VERSION_MIN_TARGET "ESPAsync_WiFiManager v1.12.2"
#define ESP_ASYNC_WIFIMANAGER_VERSION_MIN 1012002
// Use from 0 to 4. Higher number, more debugging messages and memory usage.
#define _ESPASYNC_WIFIMGR_LOGLEVEL_ 3

#include <esp_wifi.h>
#include <WiFi.h>
// From v1.1.1
#include <WiFiMulti.h>

#include <ESPAsync_WiFiManager.hpp> //https://github.com/khoih-prog/ESPAsync_WiFiManager#howto-fix-multiple-de
#include "FS.h"
#include <LittleFS.h> // https://github.com/espressif/arduino-esp32/tree/master/libraries/LittleFS

#define FileFS LittleFS
#define FS_Name "LittleFS"
#define ESP_getChipId() ((uint32_t)ESP.getEfuseMac())

#define PIN_D3 3 // Pin D3 mapped to pin GPIO3/RX0 of ESP32
#define LED_BUILTIN 2
#define LED_ON HIGH
#define LED_OFF LOW

#define MIN_AP_PASSWORD_SIZE 8

#define SSID_MAX_LEN 32
// From v1.0.10, WPA2 passwords can be up to 63 characters long.
#define PASS_MAX_LEN 64

// WifiMulti STA connection timeouts
#if (USING_ESP32_S2 || USING_ESP32_C3)
#define WIFI_MULTI_1ST_CONNECT_WAITING_MS 500L
#else
// For ESP32 core v1.0.6, must be >= 500
#define WIFI_MULTI_1ST_CONNECT_WAITING_MS 800L
#endif

#define WIFI_MULTI_CONNECT_WAITING_MS 500L

    typedef struct
{
       char wifi_ssid[SSID_MAX_LEN];
       char wifi_pw[PASS_MAX_LEN];
} WiFi_Credentials;

typedef struct
{
       String wifi_ssid;
       String wifi_pw;
} WiFi_Credentials_String;

#define NUM_WIFI_CREDENTIALS 2

// Assuming max 49 chars
#define TZNAME_MAX_LEN 50
#define TIMEZONE_MAX_LEN 50

#define USE_CONFIGURABLE_DNS false
#define USE_CUSTOM_AP_IP true

// Use false if you don't like to display Available Pages in Information Page of Config Portal
// Comment out or use true to display Available Pages in Information Page of Config Portal
// Must be placed before #include <ESP_WiFiManager.h>
#define USE_AVAILABLE_PAGES true

// From v1.0.10 to permit disable/enable StaticIP configuration in Config Portal from sketch. Valid only if DHCP is used.
// You'll loose the feature of dynamically changing from DHCP to static IP, or vice versa
// You have to explicitly specify false to disable the feature.
//#define USE_STATIC_IP_CONFIG_IN_CP          false

// Use false to disable NTP config. Advisable when using Cellphone, Tablet to access Config Portal.
// See Issue 23: On Android phone ConfigPortal is unresponsive (https://github.com/khoih-prog/ESP_WiFiManager/issues/23)
#define USE_ESP_WIFIMANAGER_NTP false


/*
// Just use enough to save memory. On ESP8266, can cause blank ConfigPortal screen
// if using too much memory
#define USING_AFRICA false
#define USING_AMERICA false
#define USING_ANTARCTICA false
#define USING_ASIA false
#define USING_ATLANTIC false
#define USING_AUSTRALIA false
#define USING_EUROPE true
#define USING_INDIAN false
#define USING_PACIFIC false
#define USING_ETC_GMT false
*/
// Use true to enable CloudFlare NTP service. System can hang if you don't have Internet access while accessing CloudFlare
// See Issue #21: CloudFlare link in the default portal (https://github.com/khoih-prog/ESP_WiFiManager/issues/21)
#define USE_CLOUDFLARE_NTP false

// New in v1.0.11
// Keeping this to false, compile time errors otherwise.
#define USING_CORS_FEATURE false

typedef struct
{
       WiFi_Credentials WiFi_Creds[NUM_WIFI_CREDENTIALS];
       char TZ_Name[TZNAME_MAX_LEN]; // "America/Toronto"
       char TZ[TIMEZONE_MAX_LEN];    // "EST5EDT,M3.2.0,M11.1.0"
       uint16_t checksum;
} WM_Config;

#define CONFIG_FILENAME F("/wifi_cred.dat")

// Use USE_DHCP_IP == true for dynamic DHCP IP, false to use static IP which you have to change accordingly to your network
#if (defined(USE_STATIC_IP_CONFIG_IN_CP) && !USE_STATIC_IP_CONFIG_IN_CP)
// Force DHCP to be true
#if defined(USE_DHCP_IP)
#undef USE_DHCP_IP
#endif
#define USE_DHCP_IP true
#else
// You can select DHCP or Static IP here
#define USE_DHCP_IP true
//#define USE_DHCP_IP     false
#endif

#if (USE_DHCP_IP)
#if (_ESPASYNC_WIFIMGR_LOGLEVEL_ > 3)
#warning Using DHCP IP
#endif
#define STATION_IP IPAddress(0, 0, 0, 0)
#define GATEWAY_IP IPAddress(192, 168, 2, 1)
#define NETMASK IPAddress(255, 255, 255, 0)

#else
#if (_ESPASYNC_WIFIMGR_LOGLEVEL_ > 3)
#warning Using static IP
#endif
#define STATION_IP IPAddress(192, 168, 2, 232)
#define GATEWAY_IP IPAddress(192, 168, 2, 1)
#define NETMASK IPAddress(255, 255, 255, 0)
#endif // if defined (USE_DHCP_IP)

#define DNS_1_IP GATEWAY_IP;
#define DNS_2_IP IPAddress(8, 8, 8, 8)
#define AP_STATIC_IP IPAddress(192, 168, 100, 1)
#define AP_STATIC_GW IPAddress(192, 168, 100, 1)
#define AP_STATIC_SN IPAddress(255, 255, 255, 0)

class WiFiManager : public WiFiMulti
{
public:
       WiFiManager(void);
       void setup(EspressoWebServer *);
       // SSID and PW for Config Portal

private:
       WM_Config WM_config;
       FS *filesystem;
       EspressoWebServer *myServer;
       DNSServer *dnsServer;
       WiFiMulti *wifiMulti;
       const int TRIGGER_PIN = PIN_D3; // Pin D3 mapped to pin GPIO03/ADC1-2/TOUCH3 of ESP32-S2

       String ssid = "ESP_" + String(ESP_getChipId(), HEX);
       const char *password = CONFIG_NETWORK_PASSWORD;

       // SSID and PW for your Router
       String Router_SSID;
       String Router_Pass;

       // Indicates whether ESP has WiFi credentials saved from previous session, or double reset detected
       bool initialConfig = false;

       WiFi_AP_IPConfig WM_AP_IPconfig;
       WiFi_STA_IPConfig WM_STA_IPconfig;

       bool loadConfigData();
       int calcChecksum(uint8_t *, uint16_t);
       void displayIPConfigStruct(WiFi_STA_IPConfig);
       void configWiFi(WiFi_STA_IPConfig);
       void saveConfigData();
       uint8_t connectMultiWiFi();

};

#endif