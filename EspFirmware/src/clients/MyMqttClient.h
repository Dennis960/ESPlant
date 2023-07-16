#ifndef __MYMQTTCLIENT_H__
#define __MYMQTTCLIENT_H__

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "MyEeprom.h"

class MyMqttClient
{
private:
    String mqttServer;
    int mqttPort;
    String mqttUser;
    String mqttPassword;
    String mqttTopic;
    String mqttClientId;
    PubSubClient mqttClient;
    WiFiClient wifiClient;

public:
    MyMqttClient(WiFiClient wifiClient);
    void setup(String mqttServer, int mqttPort, String mqttUser, String mqttPassword, String mqttTopic, String mqttClientId);
    /**
     * Returns true if the credentials in the EEPROM are valid
    */
    bool loadCredentialsFromEeprom();
    void loadDefaultCredentials();
    /**
     * Saves the credentials set in "setup" to the EEPROM
    */
    void saveCredentialsToEeprom();
    /**
     * Returns true if the message was published successfully
    */
    bool publish(String message);
};

#endif