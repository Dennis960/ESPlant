#include "plantstore.h"

#include <string.h>

#include "nvs_flash.h"
#include "esp_log.h"

bool plantstore_isInitialized = false;

void plantstore_init()
{
    ESP_ERROR_CHECK(nvs_flash_init());
}

nvs_handle_t plantstore_openNvsReadOnly()
{
    if (!plantstore_isInitialized)
    {
        plantstore_init();
        plantstore_isInitialized = true;
    }
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("plantstore", NVS_READONLY, &nvs_handle));
    return nvs_handle;
}

nvs_handle_t plantstore_openNvsReadWrite()
{
    if (!plantstore_isInitialized)
    {
        plantstore_init();
        plantstore_isInitialized = true;
    }
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("plantstore", NVS_READWRITE, &nvs_handle));
    return nvs_handle;
}

bool plantstore_getWifiCredentials(char *ssid, char *password, size_t ssid_size, size_t password_size)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadOnly();

    esp_err_t ssid_err = nvs_get_str(nvs_handle, CREDENTIALS_SSID_KEY, ssid, &ssid_size);
    esp_err_t password_err = nvs_get_str(nvs_handle, CREDENTIALS_PASSWORD_KEY, password, &password_size);

    nvs_close(nvs_handle);

    return ssid_err == ESP_OK && password_err == ESP_OK;
}

void plantstore_setWifiCredentials(char *ssid, char *password)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadWrite();
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, CREDENTIALS_SSID_KEY, ssid));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, CREDENTIALS_PASSWORD_KEY, password));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

bool plantstore_getCloudConfigurationHttp(char *url, char *auth, size_t url_size, size_t auth_size)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadOnly();

    esp_err_t url_err = nvs_get_str(nvs_handle, HTTP_URL_KEY, url, &url_size);
    esp_err_t auth_err = nvs_get_str(nvs_handle, HTTP_AUTH_KEY, auth, &auth_size);

    nvs_close(nvs_handle);

    return url_err == ESP_OK && auth_err == ESP_OK;
}

void plantstore_setCloudConfigurationHttp(char *url, char *auth)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadWrite();
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, HTTP_URL_KEY, url));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, HTTP_AUTH_KEY, auth));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

bool plantstore_getCloudConfigurationMqtt(char *server, int16_t *port, char *username, char *password, char *topic, char *clientId, size_t server_size, size_t username_size, size_t password_size, size_t topic_size, size_t clientId_size)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadOnly();

    esp_err_t server_err = nvs_get_str(nvs_handle, MQTT_SERVER_KEY, server, &server_size);
    esp_err_t port_err = nvs_get_i16(nvs_handle, MQTT_PORT_KEY, port);
    esp_err_t username_err = nvs_get_str(nvs_handle, MQTT_USERNAME_KEY, username, &username_size);
    esp_err_t password_err = nvs_get_str(nvs_handle, MQTT_PASSWORD_KEY, password, &password_size);
    esp_err_t topic_err = nvs_get_str(nvs_handle, MQTT_TOPIC_KEY, topic, &topic_size);
    esp_err_t clientId_err = nvs_get_str(nvs_handle, MQTT_CLIENTID_KEY, clientId, &clientId_size);

    nvs_close(nvs_handle);

    return server_err == ESP_OK && port_err == ESP_OK && username_err == ESP_OK &&
           password_err == ESP_OK && topic_err == ESP_OK && clientId_err == ESP_OK;
}

void plantstore_setCloudConfigurationMqtt(char *server, int16_t port, char *username, char *password, char *topic, char *clientId)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadWrite();
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, MQTT_SERVER_KEY, server));
    ESP_ERROR_CHECK(nvs_set_i16(nvs_handle, MQTT_PORT_KEY, port));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, MQTT_USERNAME_KEY, username));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, MQTT_PASSWORD_KEY, password));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, MQTT_TOPIC_KEY, topic));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, MQTT_CLIENTID_KEY, clientId));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

bool plantstore_getCloudConfigurationBlumy(char *token, size_t token_size)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadOnly();

    esp_err_t token_err = nvs_get_str(nvs_handle, BLUMY_TOKEN_KEY, token, &token_size);

    nvs_close(nvs_handle);

    return token_err == ESP_OK;
}

void plantstore_setCloudConfigurationBlumy(char *token)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadWrite();
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, BLUMY_TOKEN_KEY, token));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

bool plantstore_getSensorId(int32_t *sensorId)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadOnly();
    esp_err_t sensorId_err = nvs_get_i32(nvs_handle, SENSOR_ID_KEY, sensorId);
    nvs_close(nvs_handle);

    return sensorId_err == ESP_OK;
}

void plantstore_setSensorId(int32_t sensorId)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadWrite();
    ESP_ERROR_CHECK(nvs_set_i32(nvs_handle, SENSOR_ID_KEY, sensorId));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

bool plantstore_getSensorTimeoutSleepMs(int32_t *timeoutMs)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadOnly();
    esp_err_t timeout_err = nvs_get_i32(nvs_handle, SENSOR_TIMEOUT_SLEEP_KEY, timeoutMs);
    nvs_close(nvs_handle);

    return timeout_err == ESP_OK;
}

void plantstore_setSensorTimeoutSleepMs(int32_t timeoutMs)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadWrite();
    ESP_ERROR_CHECK(nvs_set_u32(nvs_handle, SENSOR_TIMEOUT_SLEEP_KEY, timeoutMs));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

bool plantstore_getConfigurationModeTimeoutMs(int32_t *timeoutMs)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadOnly();
    esp_err_t timeout_err = nvs_get_i32(nvs_handle, SENSOR_TIMEOUT_CONFIG_KEY, timeoutMs);
    nvs_close(nvs_handle);

    return timeout_err == ESP_OK;
}

void plantstore_setConfigurationModeTimeoutMs(int32_t timeoutMs)
{
    nvs_handle_t nvs_handle = plantstore_openNvsReadWrite();
    ESP_ERROR_CHECK(nvs_set_i32(nvs_handle, SENSOR_TIMEOUT_CONFIG_KEY, timeoutMs));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

bool plantstore_isConfigured()
{
    // Doing this is fine, because the parameters can be NULL and then the length of the stored values is checked only
    return plantstore_getWifiCredentials(NULL, NULL, 0, 0) &&
           (plantstore_getCloudConfigurationHttp(NULL, NULL, 0, 0) ||
            plantstore_getCloudConfigurationMqtt(NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0) ||
            plantstore_getCloudConfigurationBlumy(NULL, 0));
}