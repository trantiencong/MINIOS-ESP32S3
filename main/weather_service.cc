#include "weather_service.h"

#include <esp_http_client.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_crt_bundle.h>
#include <cJSON.h>
#include <cstdio>

#define TAG "WeatherService"

#ifndef CONFIG_OPENWEATHER_API_KEY
#define CONFIG_OPENWEATHER_API_KEY ""
#endif

#ifndef CONFIG_OPENWEATHER_LAT
#define CONFIG_OPENWEATHER_LAT "13.7820"
#endif

#ifndef CONFIG_OPENWEATHER_LON
#define CONFIG_OPENWEATHER_LON "109.2197"
#endif

#ifndef CONFIG_OPENWEATHER_LOCATION_NAME
#define CONFIG_OPENWEATHER_LOCATION_NAME "Quy Nhon"
#endif

#ifndef CONFIG_OPENWEATHER_LANG
#define CONFIG_OPENWEATHER_LANG "vi"
#endif

static esp_err_t HttpEventHandler(esp_http_client_event_t *evt) {
    auto* buffer = static_cast<std::string*>(evt->user_data);
    if (evt->event_id == HTTP_EVENT_ON_DATA && evt->data && evt->data_len > 0 && buffer) {
        buffer->append(static_cast<const char*>(evt->data), evt->data_len);
    }
    return ESP_OK;
}

bool WeatherService::HttpGet(const std::string& url, std::string& response) {
    response.clear();

    esp_http_client_config_t config = {};
    config.url = url.c_str();
    config.method = HTTP_METHOD_GET;
    config.timeout_ms = 10000;
    config.event_handler = HttpEventHandler;
    config.user_data = &response;
    config.disable_auto_redirect = false;
    config.crt_bundle_attach = esp_crt_bundle_attach;

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == nullptr) {
        ESP_LOGE(TAG, "esp_http_client_init failed");
        return false;
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP perform failed: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return false;
    }

    int status_code = esp_http_client_get_status_code(client);
    esp_http_client_cleanup(client);

    if (status_code != 200) {
        ESP_LOGE(TAG, "HTTP status code: %d", status_code);
        return false;
    }

    return true;
}

bool WeatherService::ParseOneCallResponse(const std::string& response, WeatherInfo& out_info) {
    cJSON* root = cJSON_Parse(response.c_str());
    if (root == nullptr) {
        ESP_LOGE(TAG, "cJSON_Parse failed");
        return false;
    }

    cJSON* current = cJSON_GetObjectItemCaseSensitive(root, "current");
    if (!cJSON_IsObject(current)) {
        cJSON_Delete(root);
        return false;
    }

    cJSON* temp = cJSON_GetObjectItemCaseSensitive(current, "temp");
    cJSON* weather_array = cJSON_GetObjectItemCaseSensitive(current, "weather");
    if (!cJSON_IsArray(weather_array) || cJSON_GetArraySize(weather_array) <= 0) {
        cJSON_Delete(root);
        return false;
    }

    cJSON* weather0 = cJSON_GetArrayItem(weather_array, 0);
    cJSON* description = cJSON_GetObjectItemCaseSensitive(weather0, "description");
    cJSON* icon = cJSON_GetObjectItemCaseSensitive(weather0, "icon");

    if (!cJSON_IsNumber(temp) || !cJSON_IsString(description) || !cJSON_IsString(icon)) {
        cJSON_Delete(root);
        return false;
    }

    char temp_text[32];
    snprintf(temp_text, sizeof(temp_text), "%.0f°C", temp->valuedouble);

    out_info.location = CONFIG_OPENWEATHER_LOCATION_NAME;
    out_info.description = description->valuestring ? description->valuestring : "";
    out_info.temperature = temp_text;
    out_info.icon_code = icon->valuestring ? icon->valuestring : "";
    out_info.fetched_at_ms = esp_timer_get_time() / 1000;
    out_info.valid = true;

    cJSON_Delete(root);
    return true;
}

bool WeatherService::FetchCurrent(WeatherInfo& out_info) {
    if (std::string(CONFIG_OPENWEATHER_API_KEY).empty()) {
        ESP_LOGE(TAG, "CONFIG_OPENWEATHER_API_KEY is empty");
        return false;
    }

    char url[512];
    snprintf(
        url,
        sizeof(url),
        "https://api.openweathermap.org/data/3.0/onecall?lat=%s&lon=%s&exclude=minutely,hourly,daily,alerts&units=metric&lang=%s&appid=%s",
        CONFIG_OPENWEATHER_LAT,
        CONFIG_OPENWEATHER_LON,
        CONFIG_OPENWEATHER_LANG,
        CONFIG_OPENWEATHER_API_KEY
    );

    std::string response;
    if (!HttpGet(url, response)) {
        return false;
    }

    return ParseOneCallResponse(response, out_info);
}