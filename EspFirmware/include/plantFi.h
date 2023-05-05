#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>

#include <ArduinoJson.h>
#include "config.h"

#define SSID "OpenWrt"

#define URL "https://esplant.hoppingadventure.com/api/data"

// The ESP8266 RTC memory is arranged into blocks of 4 bytes. The access methods read and write 4 bytes at a time,
// so the RTC data structure should be padded to a 4-byte multiple.
struct
{
    uint32_t crc32;   // 4 bytes
    uint8_t channel;  // 1 byte,   5 in total
    uint8_t bssid[6]; // 6 bytes, 11 in total
    uint8_t padding;  // 1 byte,  12 in total
} rtcData;

// Function to calculate the CRC32 of the data in the RTC memory
uint32_t calculateCRC32(const uint8_t *data, size_t length)
{
    uint32_t crc = 0xffffffff;
    while (length--)
    {
        uint8_t c = *data++;
        for (uint32_t i = 0x80; i > 0; i >>= 1)
        {
            bool bit = crc & 0x80000000;
            if (c & i)
            {
                bit = !bit;
            }

            crc <<= 1;
            if (bit)
            {
                crc ^= 0x04c11db7;
            }
        }
    }

    return crc;
}

class PlantFi
{
private:
    HTTPClient http;
    WiFiClientSecure wifiClient;

    String dataUrl = URL;

public:
    PlantFi()
    {
    }

    /**
     * @return True if the connection was established, false if a timeout occurred.
     */
    bool startWifiConnection()
    {
        // Try to read WiFi settings from RTC memory
        bool rtcValid = false;
        if (ESP.rtcUserMemoryRead(0, (uint32_t *)&rtcData, sizeof(rtcData)))
        {
            // Calculate the CRC of what we just read from RTC memory, but skip the first 4 bytes as that's the checksum itself.
            uint32_t crc = calculateCRC32(((uint8_t *)&rtcData) + 4, sizeof(rtcData) - 4);
            if (crc == rtcData.crc32)
            {
                rtcValid = true;
            }
        }
        if (rtcValid)
        {
            // The RTC data was good, make a quick connection
            WiFi.begin(SSID, NULL, rtcData.channel, rtcData.bssid, true);
        }
        else
        {
            // The RTC data was not valid, so make a regular connection
            WiFi.begin(SSID, NULL);
        }

        int retries = 0;
        int wifiStatus = WiFi.status();
        while (wifiStatus != WL_CONNECTED)
        {
            retries++;
            if (retries == 100)
            {
                // Quick connect is not working, reset WiFi and try regular connection
                WiFi.disconnect();
                delay(10);
                WiFi.forceSleepBegin();
                delay(10);
                WiFi.forceSleepWake();
                delay(10);
                WiFi.begin(SSID);
            }
            if (retries == 600)
            {
                // Giving up after 30 seconds
                WiFi.disconnect(true);
                delay(1);
                WiFi.mode(WIFI_OFF);
                return false;
            }
            delay(50);
            wifiStatus = WiFi.status();
        }

        // Write current connection info back to RTC
        rtcData.channel = WiFi.channel();
        memcpy(rtcData.bssid, WiFi.BSSID(), 6); // Copy 6 bytes of BSSID (AP's MAC address)
        rtcData.crc32 = calculateCRC32(((uint8_t *)&rtcData) + 4, sizeof(rtcData) - 4);
        ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtcData, sizeof(rtcData));
        return true;
    }

    /**
     * Sends the sensor data to the api
     * @param sensorAddress The address of the sensor
     * @param water The water value of the sensor
     */
    void sendData(int sensorAddress, int water, unsigned long start, uint16_t voltage)
    {
        wifiClient.setInsecure();
        http.begin(wifiClient, dataUrl);
        http.addHeader("Content-Type", "application/json");
        StaticJsonDocument<200> doc;
        doc["sensorAddress"] = sensorAddress;
        doc["water"] = water;
        doc["duration"] = millis() - start;
        doc["voltage"] = voltage;
        String payload;
        serializeJson(doc, payload);
        http.POST(payload);
        http.end();
    }
};