#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_littlefs.h"

void initLittleFs()
{
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/configuration_server",
        .partition_label = "configuration_server",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };
    ESP_ERROR_CHECK(esp_vfs_littlefs_register(&conf));
    size_t total = 0, used = 0;
    esp_err_t ret = esp_littlefs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE("LITTLE_FS", "Failed to get LittleFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI("LITTLE_FS", "Partition size: total: %d, used: %d", total, used);
    }
}

void deinitLittleFs()
{
    ESP_ERROR_CHECK(esp_vfs_littlefs_unregister("/configuration_server"));
}

// Returns the requested file from the LittleFS
esp_err_t get_handler(httpd_req_t *req)
{
    char path[600];
    sprintf(path, "/configuration_server%s", req->uri);
    ESP_LOGI("HTTP", "GET %s", path);
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        httpd_resp_sendstr_chunk(req, line);
    }
    fclose(file);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t post_api_connect_handler(httpd_req_t *req)
{
    char content[100];

    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    /* Send a simple response */
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t post_api_reset_handler(httpd_req_t *req)
{
    char content[100];

    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    /* Send a simple response */
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t get_api_networks_handler(httpd_req_t *req)
{
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t get_api_isConnected_handler(httpd_req_t *req)
{
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t post_api_mqttSetup_handler(httpd_req_t *req)
{
    char content[100];

    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    /* Send a simple response */
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t post_api_sensorId_handler(httpd_req_t *req)
{
    char content[100];

    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    /* Send a simple response */
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t get_api_sensorId_handler(httpd_req_t *req)
{
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t post_api_timeouts_sleep_handler(httpd_req_t *req)
{
    char content[100];

    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    /* Send a simple response */
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t get_api_timeouts_sleep_handler(httpd_req_t *req)
{
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t get_api_update_percentage_handler(httpd_req_t *req)
{
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t post_api_plantName_handler(httpd_req_t *req)
{
    char content[100];

    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(content));

    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0)
    {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT)
        {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    /* Send a simple response */
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t get_api_plantName_handler(httpd_req_t *req)
{
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t get_api_connectedNetwork_handler(httpd_req_t *req)
{
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t get_api_sensor_value_handler(httpd_req_t *req)
{
    const char resp[] = "Not Implemented";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

httpd_uri_t get = {
    .uri = "/*",
    .method = HTTP_GET,
    .handler = get_handler,
    .user_ctx = NULL};

httpd_uri_t post_api_connect = {
    .uri = "/api/connect",
    .method = HTTP_POST,
    .handler = post_api_connect_handler,
    .user_ctx = NULL};

httpd_uri_t post_api_reset = {
    .uri = "/api/reset",
    .method = HTTP_POST,
    .handler = post_api_reset_handler,
    .user_ctx = NULL};

httpd_uri_t get_api_networks = {
    .uri = "/api/networks",
    .method = HTTP_GET,
    .handler = get_api_networks_handler,
    .user_ctx = NULL};

httpd_uri_t get_api_isConnected = {
    .uri = "/api/isConnected",
    .method = HTTP_GET,
    .handler = get_api_isConnected_handler,
    .user_ctx = NULL};

httpd_uri_t post_api_mqttSetup = {
    .uri = "/api/mqttSetup",
    .method = HTTP_POST,
    .handler = post_api_mqttSetup_handler,
    .user_ctx = NULL};

httpd_uri_t post_api_sensorId = {
    .uri = "/api/sensorId",
    .method = HTTP_POST,
    .handler = post_api_sensorId_handler,
    .user_ctx = NULL};

httpd_uri_t get_api_sensorId = {
    .uri = "/api/sensorId",
    .method = HTTP_GET,
    .handler = get_api_sensorId_handler,
    .user_ctx = NULL};

httpd_uri_t post_api_timeouts_sleep = {
    .uri = "/api/timeouts/sleep",
    .method = HTTP_POST,
    .handler = post_api_timeouts_sleep_handler,
    .user_ctx = NULL};

httpd_uri_t get_api_timeouts_sleep = {
    .uri = "/api/timeouts/sleep",
    .method = HTTP_GET,
    .handler = get_api_timeouts_sleep_handler,
    .user_ctx = NULL};

httpd_uri_t get_api_update_percentage = {
    .uri = "/api/update/percentage",
    .method = HTTP_GET,
    .handler = get_api_update_percentage_handler,
    .user_ctx = NULL};

httpd_uri_t post_api_plantName = {
    .uri = "/api/plantName",
    .method = HTTP_POST,
    .handler = post_api_plantName_handler,
    .user_ctx = NULL};

httpd_uri_t get_api_plantName = {
    .uri = "/api/plantName",
    .method = HTTP_GET,
    .handler = get_api_plantName_handler,
    .user_ctx = NULL};

httpd_uri_t get_api_connectedNetwork = {
    .uri = "/api/connectedNetwork",
    .method = HTTP_GET,
    .handler = get_api_connectedNetwork_handler,
    .user_ctx = NULL};

httpd_uri_t get_api_sensorValue = {
    .uri = "/api/sensor/value",
    .method = HTTP_GET,
    .handler = get_api_sensor_value_handler,
    .user_ctx = NULL};

/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
    /* Generate default configuration */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    /* Empty handle to esp_http_server */
    httpd_handle_t server = NULL;

    /* Start the httpd server */
    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &post_api_connect);
        httpd_register_uri_handler(server, &post_api_reset);
        httpd_register_uri_handler(server, &get_api_networks);
        httpd_register_uri_handler(server, &get_api_isConnected);
        httpd_register_uri_handler(server, &post_api_mqttSetup);
        httpd_register_uri_handler(server, &post_api_sensorId);
        httpd_register_uri_handler(server, &get_api_sensorId);
        httpd_register_uri_handler(server, &post_api_timeouts_sleep);
        httpd_register_uri_handler(server, &get_api_timeouts_sleep);
        httpd_register_uri_handler(server, &get_api_update_percentage);
        httpd_register_uri_handler(server, &post_api_plantName);
        httpd_register_uri_handler(server, &get_api_plantName);
        httpd_register_uri_handler(server, &get_api_connectedNetwork);
        httpd_register_uri_handler(server, &get_api_sensorValue);
        httpd_register_uri_handler(server, &get);
    }
    /* If server failed to start, handle will be NULL */
    return server;
}

/* Function for stopping the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server)
    {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}