#ifndef __MOISTURESENSOR_H__
#define __MOISTURESENSOR_H__

#include <Arduino.h>
#include "Sensor.h"
#include "configuration/Config.h"

class MoistureSensor : public Sensor
{
private:
    int numberOfValues = 50;
    int values[50];
    long sum = 0;
    int counter = 0;

public:
    void enable() override;
    void disable() override;
    int getRawValue() override;
    float getComputedValue() override;
    /**
     * Needs to be called in every loop.
     */
    void loop();
};
#endif // __MOISTURESENSOR_H__