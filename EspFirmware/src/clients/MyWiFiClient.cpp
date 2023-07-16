#include "MyWiFiClient.h"

MyWiFiClient::MyWiFiClient()
{
}

void MyWiFiClient::setup(String ssid, String password)
{
    this->ssid = ssid;
    this->password = password;
}

void MyWiFiClient::loop()
{
    if (!shouldConnect)
    {
        return;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        if (!connected)
        {
            connected = true;
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            saveSettingsToRtc();
        }
    }
    else
    {
        if (connected)
        {
            connected = false;
            Serial.println("WiFi disconnected");
            if (shouldConnect)
            {
                connect();
            }
        }
        unsigned long now = millis();
        if (now - lastConnectAttempt > QUICK_CONNECT_TIMEOUT)
        {
            Serial.println("Connecting to WiFi...");
            connect(false);
        }
    }
}

void MyWiFiClient::connect(bool quickConnect)
{
    if (firstConnectAttempt == 0)
    {
        firstConnectAttempt = millis();
    }
    lastConnectAttempt = millis();
    if (quickConnect && loadSettingsFromRtc())
    {
        WiFi.begin(ssid, password, rtcData.channel, rtcData.bssid, true);
    }
    else
    {
        WiFi.begin(ssid, password);
    }
}

void MyWiFiClient::disconnect()
{
    WiFi.disconnect();
}

bool MyWiFiClient::isConnected()
{
    return connected;
}

bool MyWiFiClient::loadCredentialsFromEeprom()
{
    if (isWifiChecksumValid())
    {
        loadWiFiCredentials(ssid, password);
        return true;
    }
    else
    {
        return false;
    }
}

void MyWiFiClient::saveCredentialsToEeprom()
{
    saveWiFiCredentials(ssid, password);
}

bool MyWiFiClient::loadSettingsFromRtc()
{
    return ESP.rtcUserMemoryRead(0, (uint32_t *)&rtcData, sizeof(rtcData)) && isRtcValid();
}

void MyWiFiClient::saveSettingsToRtc()
{
    rtcData.channel = WiFi.channel();
    memcpy(rtcData.bssid, WiFi.BSSID(), 6); // Copy 6 bytes of BSSID (AP's MAC address)
    rtcData.crc32 = calculateCRC32(((uint8_t *)&rtcData) + 4, sizeof(rtcData) - 4);
    ESP.rtcUserMemoryWrite(0, (uint32_t *)&rtcData, sizeof(rtcData));
}

bool MyWiFiClient::isRtcValid()
{
    // Calculate the CRC of what we just read from RTC memory, but skip the first 4 bytes as that's the checksum itself.
    uint32_t crc = calculateCRC32(((uint8_t *)&rtcData) + 4, sizeof(rtcData) - 4);
    return crc == rtcData.crc32;
}

bool MyWiFiClient::isWifiTimeout()
{
    return millis() - firstConnectAttempt > WIFI_TIMEOUT;
}

int MyWiFiClient::getRssi()
{
    return WiFi.RSSI();
}
