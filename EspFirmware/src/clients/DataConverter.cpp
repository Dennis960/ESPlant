#include "DataConverter.h"

String dataToString(int sensorAddress, String plantName, int rssi, float water, unsigned long measurementDuration)
{
    String data = "{\"sensorAddress\":";
    data += sensorAddress;
    data += ",\"plantName\":\"";
    data += plantName;
    data += "\",\"rssi\":";
    data += rssi;
    data += ",\"water\":";
    data += water;
    data += ",\"measurementDuration\":";
    data += measurementDuration;
    data += "}";
    return data;
}
