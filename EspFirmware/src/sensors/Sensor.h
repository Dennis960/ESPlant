#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <Arduino.h>

class Sensor
{
protected:
    bool enabled = false;
    unsigned long enableTime = 0;
    unsigned long disableTime = 0;
    int value = 0;
    bool ready = false;

public:
    Sensor() {}
    virtual ~Sensor() {}
    /**
     * Enables the sensor.
     *
     * When overriding this method, make sure to call the base implementation.
     */
    virtual void enable();
    /**
     * Disables the sensor.
     *
     * When overriding this method, make sure to call the base implementation.
     */
    virtual void disable();
    /**
     * Returns the raw value of the measurement.
     *
     * The value will be between 0 and ANALOG_MAX.
     */
    virtual int getRawValue();
    /**
     * Returns the computed value of the measurement.
     *
     * The value will be in form of a standard unit matching the sensor type (e.g. °C for temperature).
     */
    virtual float getComputedValue() = 0;
    /**
     * Returns the time the sensor was active in milliseconds.
     *
     * If the sensor is currently active, the time since the sensor was enabled is returned.
     * If the sensor is currently disabled, the time between the last enable and the last disable is returned.
     */
    virtual int getActiveTime() final;
    /**
     * Returns whether the sensor is ready to be read.
     */
    virtual bool isReady();
};

#endif // __SENSOR_H__