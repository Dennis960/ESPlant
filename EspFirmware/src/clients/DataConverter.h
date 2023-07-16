#ifndef __DATACONVERTER_H__
#define __DATACONVERTER_H__

#include <Arduino.h>

String dataToString(int sensorAddress, String plantName, int rssi, float water, unsigned long measurementDuration);

#endif // __MYDATA_H__