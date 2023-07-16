#include "Sensor.h"

void Sensor::enable()
{
    enableTime = millis();
    enabled = true;
}
void Sensor::disable()
{
    disableTime = millis();
    enabled = false;
}

bool Sensor::isReady()
{
    return ready;
}

int Sensor::getRawValue()
{
    return value;
}

int Sensor::getActiveTime()
{
    if (enabled)
    {
        return millis() - enableTime;
    }
    else
    {
        return disableTime - enableTime;
    }
}
