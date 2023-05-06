#include <Arduino.h>

#define DEBUG

#ifndef MY_SERIAL_H
#define MY_SERIAL_H

void inline serialPrintf(const char *format, ...)
{
    #ifdef DEBUG
    char buf[256]; // resulting string limited to 256 chars
    va_list args;
    va_start(args, format);
    vsnprintf(buf, 256, format, args);
    va_end(args);
    Serial.printf("%04lu: ", millis());
    Serial.print(buf);
    #endif
}
#endif