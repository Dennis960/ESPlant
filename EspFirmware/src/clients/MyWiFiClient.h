#ifndef __MYWIFICLIENT_H__
#define __MYWIFICLIENT_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "MyEeprom.h"
#include "Utils.h"

class MyWiFiClient
{
private:
    String ssid;
    String password;
    WiFiClient wifiClient;
    bool connected = false;
    unsigned long lastConnectAttempt = 0;
    unsigned long firstConnectAttempt = 0;
    bool shouldConnect = false;

    /**
     * The data structure to store the WiFi settings in the RTC memory.
     *
     * The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods read and write 4 bytes at a time,
     * so the RTC data structure should be padded to a 4-byte multiple.
     */
    struct RtcData
    {
        uint32_t crc32;   // 4 bytes
        uint8_t channel;  // 1 byte,   5 in total
        uint8_t bssid[6]; // 6 bytes, 11 in total
        uint8_t padding;  // 1 byte,  12 in total
    };
    RtcData rtcData;

    /**
     * Checks if the RTC data is valid
     * @return True if the RTC data is valid, false otherwise
     */
    bool isRtcValid();

    /**
     * Loads the credentials from the RTC memory
     * @return True if the credentials are valid, false otherwise
     */
    bool loadSettingsFromRtc();
    void saveSettingsToRtc();

public:
    MyWiFiClient();
    void setup(String ssid, String password);
    void loop();
    void connect(bool quickConnect = true);
    void disconnect();
    bool isConnected();
    /**
     * Loads the credentials from the EEPROM
     * @return True if the credentials are valid, false otherwise
     */
    bool loadCredentialsFromEeprom();
    void saveCredentialsToEeprom();
    WiFiClient &getWiFiClient() { return wifiClient; };
    bool isWifiTimeout();
    int getRssi();
};
#endif // __MYWIFICLIENT_H__