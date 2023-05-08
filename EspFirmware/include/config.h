#include <ESP8266WiFi.h>

#ifndef CONFIG_H
#define CONFIG_H

//----------------- Sensor Address -----------------
/**
 * The sensorAddress value used to identify the sensor
*/
const int sensorAddress = 2;

//----------------- Measurement delays -----------------
/**
 * The sleep duration between measurements in seconds
*/
const uint64_t SLEEP_DURATION_SECONDS = 1;
/**
 * The sleep duration between measurements in microseconds
*/
const uint64_t SLEEP_DURATION = SLEEP_DURATION_SECONDS * 1000000;

//----------------- Timeouts -----------------
/**
 * The time to wait for a connection to be established using the quick connect method (bssid and channel)
 * Unit: milliseconds
*/
const unsigned long QUICK_CONNECT_TIMEOUT = 2000;
/**
 * The time to wait for a connection to be established using the regular connect method (only ssid and password)
 * Unit: milliseconds
*/
const unsigned long WIFI_TIMEOUT = 7000;
/**
 * The time to wait for the sensor to respond with a valid value
 * Sensor takes 80ms to start and 550ms to measure
 * Unit: milliseconds
*/
const unsigned long SENSOR_TIMEOUT = 1400;
/**
 * The total timeout of the esp. After this time, the esp will be forced into deep sleep
 * Unit: milliseconds
*/
const unsigned long TOTAL_TIMEOUT = 10000;

#endif