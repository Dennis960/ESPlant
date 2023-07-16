#include "MyMqttClient.h"

MyMqttClient::MyMqttClient(WiFiClient wifiClient)
{
    this->wifiClient = wifiClient;
}

void MyMqttClient::setup(String mqttServer, int mqttPort, String mqttUser, String mqttPassword, String mqttTopic, String mqttClientId)
{
    this->mqttServer = mqttServer;
    this->mqttPort = mqttPort;
    this->mqttUser = mqttUser;
    this->mqttPassword = mqttPassword;
    this->mqttTopic = mqttTopic;
    this->mqttClientId = mqttClientId;
}

bool MyMqttClient::loadCredentialsFromEeprom()
{
    if (isMqttChecksumValid())
    {
        loadMqttCredentials(mqttServer, mqttPort, mqttUser, mqttPassword, mqttTopic, mqttClientId);
        return true;
    }
    else
    {
        return false;
    }
}

void MyMqttClient::loadDefaultCredentials()
{
    mqttServer = "schneefux.xyz";
    mqttPort = 1883;
    mqttUser = "esplant";
    mqttPassword = "Ma9BdqVcKyxTgJm3";
    mqttTopic = "esplant";
    mqttClientId = "sensor-1";
}

void MyMqttClient::saveCredentialsToEeprom()
{
    saveMqttCredentials(mqttServer, mqttPort, mqttUser, mqttPassword, mqttTopic, mqttClientId);
}

bool MyMqttClient::publish(String message)
{
    
    mqttClient.setClient(wifiClient);
    mqttClient.setServer(mqttServer.c_str(), mqttPort);
    if (!mqttClient.connect(mqttClientId.c_str(), mqttUser.c_str(), mqttPassword.c_str())) {
        return false;
    }
    mqttClient.publish(mqttTopic.c_str(), message.c_str());
    mqttClient.disconnect();
    return true;
}