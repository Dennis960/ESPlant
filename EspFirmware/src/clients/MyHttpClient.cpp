#include "MyHttpClient.h"

MyHttpClient::MyHttpClient(WiFiClient wifiClient)
{
    this->wifiClient = wifiClient;
}

void MyHttpClient::setup(String server, int port, String path)
{
    this->server = server;
    this->port = port;
    this->path = path;
}

bool MyHttpClient::loadSettingsFromEeprom()
{
    // TODO: Implement
    return true;
}

void MyHttpClient::saveSettingsToEeprom()
{
    // TODO: Implement
}

bool MyHttpClient::publish(String message)
{
    // TODO: Implement
    return true;
}
