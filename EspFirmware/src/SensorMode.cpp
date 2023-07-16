#include "SensorMode.h"

MyWiFiClient wifiClient = MyWiFiClient();
MoistureSensor moistureSensor = MoistureSensor();
MyMqttClient mqttClient = MyMqttClient(wifiClient.getWiFiClient());
MyHttpClient httpClient = MyHttpClient(wifiClient.getWiFiClient());

int sensorValue = -1;
bool wasWifiConnectedLastCycle = false;

void sensorSetup()
{
    if (!wifiClient.loadCredentialsFromEeprom())
    {
        serialPrintf("EEPROM Wifi not valid, starting configuration mode\n");
        reset(CONFIGURATION_FLAG);
    }

    moistureSensor.enable();
    mqttClient.loadCredentialsFromEeprom();

    serialPrintf("Starting wifi connection\n");
    wifiClient.connect();
}

void sensorLoop()
{
    moistureSensor.loop();
    wifiClient.loop();
    if (wifiClient.isConnected() && moistureSensor.isReady())
    {
        String data = dataToString(loadSensorId(), loadPlantName(), wifiClient.getRssi(), moistureSensor.getComputedValue(), moistureSensor.getActiveTime());
        mqttClient.publish(data);
        httpClient.publish(data);
        serialPrintf("Published data: %s\n", data.c_str());
        moistureSensor.disable();
        wifiClient.disconnect();
        startDeepSleep(loadSleepDuration());
    }

    if (wifiClient.isWifiTimeout())
    {
        serialPrintf("Wifi timeout\n");
        startDeepSleep(loadSleepDuration());
    }
    if (millis() > TOTAL_TIMEOUT)
    {
        serialPrintf("Total timeout\n");
        startDeepSleep(loadSleepDuration());
    }
}