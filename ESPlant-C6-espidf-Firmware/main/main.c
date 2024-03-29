#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "peripherals/sensors.c"
#include "secret.c"
#include "wifi.c"
#include "configuration_mode_server.c"

#include "esp_http_client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

// TODO use udp or mqtt
void sendSensorData(sensors_full_data_t *sensors_data, int8_t rssi)
{
    int sensorAddress = 13;
    char data[400];

    sprintf(data, "{\"sensorAddress\":%d,\"light\":%2.2f,\"voltage\":%.2f,\"temperature\":%.2f,\"humidity\":%.2f,\"isUsbConnected\":%s,\"moisture\":%d,\"moistureStabilizationTime\":%lu,\"isMoistureMeasurementSuccessful\":%s,\"humidityRaw\":%lu,\"temperatureRaw\":%lu,\"rssi\":%d}",
            sensorAddress,
            sensors_data->light,
            sensors_data->voltage,
            sensors_data->temperature,
            sensors_data->humidity,
            sensors_data->is_usb_connected ? "true" : "false",
            sensors_data->moisture_measurement,
            sensors_data->moisture_stabilization_time,
            sensors_data->moisture_measurement_successful ? "true" : "false",
            sensors_data->humidity_raw,
            sensors_data->temperature_raw,
            rssi);

    esp_http_client_config_t config = {
        .url = SECRET_API_URL,
        .method = HTTP_METHOD_POST,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_post_field(client, data, strlen(data));
    esp_http_client_set_header(client, "Content-Type", "application/json");
    ESP_ERROR_CHECK(esp_http_client_perform(client));

    esp_http_client_cleanup(client);

    ESP_LOGI("Data", "%s", data);
    int status_code = esp_http_client_get_status_code(client);
    ESP_LOGI("HTTP", "Status Code: %d", status_code);
}

void app_main()
{
    initNvs();
    initLittleFs();

    wifi_config_t wifi_sta_config = {
        .sta = {
            .ssid = SECRET_ESP_WIFI_SSID,
            .password = SECRET_ESP_WIFI_PASS,
        },
    };
    wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid = "Blumy",
            .password = "Blumy123",
            .max_connection = 4,
        },
    };

    int8_t rssi;
    bool success = initWifi(&wifi_sta_config, &wifi_ap_config, 5, &rssi);
    if (!success)
    {
        ESP_LOGE("WIFI", "Failed to connect to wifi");
        return;
    }
    sensors_initSensors();

    sensors_full_data_t sensors_data;
    sensors_full_read(&sensors_data);

    sendSensorData(&sensors_data, rssi);

    while (1)
    {
        // Server mode
    }
    

    sensors_deinitSensors();
    deinitLittleFs();
}
