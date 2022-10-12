#pragma once
#ifndef wifiManager_h
#define wifiManager_h

#define USE_ASYNC_DNS true
#ifdef USE_ASYNC_DNS 
#include <ESPAsyncDNSServer.h>
#else
#include <DNSServer.h>
#endif
#include "ESPressoMachineDefaults.h"
#include "EspressoWebServer.h"
#include "WiFiMulti.h"

class ESPressoInterface; /// forward declaration

#define NUM_WIFI_CREDENTIALS 2
// Assuming max 49 chars
#define TZNAME_MAX_LEN 50
#define TIMEZONE_MAX_LEN 50
#define SSID_MAX_LEN 32
#define PASS_MAX_LEN 64

#define AP_PASSWORD "E32" // HEX will be added

#define MIN_AP_PASSWORD_SIZE 8
#define DNS_PORT 53
#define RFC952_HOSTNAME_MAXLEN 63 // HOSTNAME can be up to 255 chars, but we'll take DNS label length. (longer than  in original code)
#define MAX_WIFI_CHANNEL 13
#define CONFIGPORTAL_TIMEOUT 80 * 1000
#define WIFI_MULTI_CONNECT_WAITING_MS 10 * 1000 // MultiWifi reconnects after 10 seconds.
#define WIFI_MULTI_1ST_CONNECT_WAITING_MS 3 * 1000

class EspressoConfig;  // Forward declaration
class ESPressoMachine; // Foraward declaration

#include <esp_wifi.h>
#define ESP_getChipId() ((uint32_t)ESP.getEfuseMac())

#ifndef CONFIG_NETWORK_PASSWORD
CONFIG_NETWORK_PASSWORD "ESP32ressoMachine"
#endif
// Anything Wifi Goes into this class - however the configuration bits are in the config class.

#if !(defined(ESP32))
#error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
#endif

    // WifiManager related config

    typedef struct
{
       IPAddress _ap_static_ip;
       IPAddress _ap_static_gw;
       IPAddress _ap_static_sn;
} WiFi_AP_IPConfig;

typedef struct
{
       IPAddress _sta_static_ip;
       IPAddress _sta_static_gw;
       IPAddress _sta_static_sn;
       IPAddress _sta_static_dns1;
       IPAddress _sta_static_dns2;
} WiFi_STA_IPConfig;

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
typedef struct
{
       WiFi_Credentials WiFi_Creds[NUM_WIFI_CREDENTIALS];
       char TZ_Name[TZNAME_MAX_LEN]; // "America/Toronto"
       char TZ[TIMEZONE_MAX_LEN];    // "EST5EDT,M3.2.0,M11.1.0"
       uint16_t checksum;
} WM_Config;

class WiFiManager : public WiFiMulti
{
public:
       WiFiManager();
       void setupWiFiAp(WiFi_AP_IPConfig *);
       void loopPortal(ESPressoInterface *myInterface);
       uint8_t connectMultiWiFi(EspressoConfig *);
#ifdef USE_ASYNC_DNS
       AsyncDNSServer *dnsServer;
#else
       DNSServer *dnsServer;
#endif
       // SSID and PW for Config Portal

private:
       ESPressoInterface *myInterface;

       String ApSSID;
       String ApPass;

       bool connect;

       char *getRFC952_hostname(const char *);
       char RFC952_hostname[RFC952_HOSTNAME_MAXLEN + 1];
       unsigned long _configPortalStart = 0;
};

#endif
