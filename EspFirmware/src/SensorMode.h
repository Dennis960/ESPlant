#ifndef SensorMode_H
#define SensorMode_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "clients/MyWiFiClient.h"
#include "sensors/MoistureSensor.h"
#include "configuration/Config.h"
#include "Utils.h"
#include "MyEeprom.h"
#include "clients/MyMqttClient.h"
#include "clients/MyHttpClient.h"
#include "clients/DataConverter.h"

extern MyWiFiClient wifiClient;
extern MoistureSensor sensor;
extern MyMqttClient mqttClient;

/**
 * Setup function for the sensor mode
 */
void sensorSetup();

/**
 * Loop function for the sensor mode
 */
void sensorLoop();

#endif