#ifndef __MYHTTPCLIENT_H__
#define __MYHTTPCLIENT_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "MyEeprom.h"


class MyHttpClient
{
private:
    String server;
    int port;
    String path;
    WiFiClient wifiClient;

public:
    MyHttpClient(WiFiClient wifiClient);
    void setup(String server, int port, String path);
    /**
     * Returns true if the credentials in the EEPROM are valid
    */
    bool loadSettingsFromEeprom();
    /**
     * Saves the credentials set in "begin" to the EEPROM
    */
    void saveSettingsToEeprom();
    /**
     * Returns true if the message was published successfully
    */
    bool publish(String message);
};
#endif // __MYHTTPCLIENT_H__