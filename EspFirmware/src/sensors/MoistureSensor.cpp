#include "MoistureSensor.h"

MoistureSensor sensor = MoistureSensor();

void MoistureSensor::enable()
{
    Sensor::enable();
    analogWriteFreq(40000);
    pinMode(MOISTURE_SENSOR_SW_PIN, OUTPUT);
    pinMode(MOISTURE_SENSOR_ANALOG_PIN, INPUT);
    analogWrite(MOISTURE_SENSOR_SW_PIN, 1);
}

void MoistureSensor::disable()
{
    Sensor::disable();
    analogWrite(MOISTURE_SENSOR_SW_PIN, 0);
    pinMode(MOISTURE_SENSOR_SW_PIN, INPUT);
    pinMode(MOISTURE_SENSOR_ANALOG_PIN, INPUT);
}

int MoistureSensor::getRawValue()
{
    return value;
}

float MoistureSensor::getComputedValue()
{
    return (ANALOG_MAX - value) / (float)ANALOG_MAX;
}

void MoistureSensor::loop()
{
    if (!enabled)
    {
        return;
    }
    // measure analog voltage
    int v = analogRead(MOISTURE_SENSOR_ANALOG_PIN);
    sum = sum - values[counter] + v;
    values[counter] = v;
    counter = (counter + 1) % numberOfValues;
    int average = sum / numberOfValues;
    // calculate standard deviation
    int sumOfSquares = 0;
    for (int i = 0; i < numberOfValues; i++)
    {
        int diff = values[i] - average;
        sumOfSquares += diff * diff;
    }
    int standardDeviation = sqrt(sumOfSquares / numberOfValues);
    if (standardDeviation < 50 && values[numberOfValues - 1] != 0)
    {
        value = v;
        ready = true;
        enabled = false;
    }
}
