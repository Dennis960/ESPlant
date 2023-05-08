#include <Arduino.h>
#include <plantFi.h>
#include <sensor.h>
#include <ESP8266WiFi.h>

#include "myserial.h"
#include "config.h"

const int sensorI2CAddress = 1;

const int sensorEnablePin = 3; // RX
const int sdaPin = 2;
const int sclPin = 0;

Sensor sensor = Sensor(sensorI2CAddress, sensorEnablePin);
PlantFi plantFi = PlantFi();

ADC_MODE(ADC_VCC);

void startDeepSleep(uint64_t duration)
{
    serialPrintf("Going to sleep for %llu us\n", duration);
    ESP.deepSleep(duration, WAKE_RF_DISABLED);
    yield();
}

void setup()
{
    WiFi.persistent(false);
    WiFi.forceSleepBegin();
    delay(1);
    WiFi.mode(WIFI_OFF);
    delay(1);
    serialPrintf("VCC: %d\n", ESP.getVcc());
    serialPrintf("Enabling sensor\n");
    sensor.enable();
    serialPrintf("Initializing plantFi\n");
    plantFi.checkRtcValdity();

    serialPrintf("Initializing I2C\n");
    Wire.begin(sdaPin, sclPin);
    Wire.setTimeout(1000);
    Wire.setClockStretchLimit(100000);

    serialPrintf("Starting wifi connection\n");
    plantFi.connectWifi(plantFi.rtcValid);
}

const unsigned int INVALID_WATER = 65535;

unsigned int water = INVALID_WATER;

bool wasWifiConnectedLastCycle = false;

void loop()
{
    // Sensor capacitance
    if (water == INVALID_WATER && sensor.isCapacitanceAvailable())
    {
        water = sensor.getRequestedCapacitance();
        serialPrintf("Water: %u\n", water);
        if (water >= INVALID_WATER)
        {
            water = INVALID_WATER;
        }
        else
        {
            serialPrintf("Disabling sensor\n");
            sensor.disable();
        }
    }

    // Start measurement if sensor is not measuring and active
    if (sensor.isActive() && !sensor.isMeasuring)
    {
        serialPrintf("Requesting capacitance\n");
        sensor.requestCapacitance();
    }

    // Sensor timeout
    if (sensor.isActive() && millis() - sensor.sensorEnableTime > SENSOR_TIMEOUT)
    {
        serialPrintf("Sensor timeout\n");
        sensor.disable();
        water = 1; // 1 means no water measured (can't be 0 because 0 is falsy in JavaScript)
    }

    // Check wifi connection
    if (plantFi.isWifiConnected())
    {
        if (!wasWifiConnectedLastCycle)
        {
            serialPrintf("Wifi connected\n");
            wasWifiConnectedLastCycle = true;
        }
        if (!plantFi.rtcValid)
        {
            serialPrintf("Saving connection\n");
            plantFi.saveConnection();
            plantFi.rtcValid = true;
        }
        if (water != INVALID_WATER)
        {
            serialPrintf("Sending data\n");
            plantFi.sendData(sensorAddress, water, ESP.getVcc());
            startDeepSleep(SLEEP_DURATION);
        }
    }
    else
    {
        if (plantFi.rtcValid)
        {
            if (millis() - plantFi.connectionStartTime > QUICK_CONNECT_TIMEOUT)
            {
                serialPrintf("Quick connect failed, resetting wifi\n");
                plantFi.resetWifi(); // sets rtcValid to false
            }
        }
        else
        {
            if (millis() - plantFi.connectionStartTime > WIFI_TIMEOUT)
            {
                serialPrintf("Regular connect failed, giving up\n");
                startDeepSleep(SLEEP_DURATION);
            }
        }
    }

    // Total timeout
    if (millis() - plantFi.connectionStartTime > TOTAL_TIMEOUT)
    {
        serialPrintf("Total timeout\n");
        startDeepSleep(SLEEP_DURATION);
    }
}