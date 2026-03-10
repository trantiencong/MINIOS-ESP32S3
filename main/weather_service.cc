#include "weather_service.h"

#include <esp_http_client.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_crt_bundle.h>
#include <cJSON.h>
#include <cstdio>
#include <cstdlib>
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

bool WeatherService::ParseOneCallResponse(
    const std::string& response,
    const std::string& location_name,
    WeatherInfo& out_info
) {
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
    cJSON* humidity = cJSON_GetObjectItemCaseSensitive(current, "humidity");
    cJSON* weather_array = cJSON_GetObjectItemCaseSensitive(current, "weather");
    if (!cJSON_IsArray(weather_array) || cJSON_GetArraySize(weather_array) <= 0) {
        cJSON_Delete(root);
        return false;
    }

    cJSON* weather0 = cJSON_GetArrayItem(weather_array, 0);
    cJSON* description = cJSON_GetObjectItemCaseSensitive(weather0, "description");
    cJSON* icon = cJSON_GetObjectItemCaseSensitive(weather0, "icon");

    if (!cJSON_IsNumber(temp) || !cJSON_IsNumber(humidity) || !cJSON_IsString(description) || !cJSON_IsString(icon)) {
    cJSON_Delete(root);
    return false;
}

    char temp_text[32];
    char humidity_text[32];
    snprintf(temp_text, sizeof(temp_text), "%.0f°C", temp->valuedouble);
    snprintf(humidity_text, sizeof(humidity_text), "%.0f%%", humidity->valuedouble);

    out_info.location = location_name;
    out_info.description = description->valuestring ? description->valuestring : "";
    out_info.temperature = temp_text;
    out_info.humidity = humidity_text;
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

    std::string location_name = CONFIG_OPENWEATHER_LOCATION_NAME;
    std::string latitude = CONFIG_OPENWEATHER_LAT;
    std::string longitude = CONFIG_OPENWEATHER_LON;

    GeoLocationInfo geo;
    if (FetchGeoLocation(geo) && geo.valid) {
        auto localize_city = [](const std::string& city) -> std::string {
    // Major cities / municipalities
    if (city == "Hanoi") return "Hà Nội";
    if (city == "Ho Chi Minh City") return "TP. Hồ Chí Minh";
    if (city == "Da Nang") return "Đà Nẵng";
    if (city == "Hai Phong") return "Hải Phòng";
    if (city == "Can Tho") return "Cần Thơ";
    if (city == "Hue") return "Huế";
    if (city == "Nha Trang") return "Nha Trang";
    if (city == "Quy Nhon") return "Quy Nhơn";
    if (city == "Vung Tau") return "Vũng Tàu";
    if (city == "Bien Hoa") return "Biên Hòa";
    if (city == "Thu Dau Mot") return "Thủ Dầu Một";
    if (city == "Buon Ma Thuot") return "Buôn Ma Thuột";
    if (city == "Pleiku") return "Pleiku";
    if (city == "Da Lat") return "Đà Lạt";
    if (city == "Phan Thiet") return "Phan Thiết";
    if (city == "Rach Gia") return "Rạch Giá";
    if (city == "Long Xuyen") return "Long Xuyên";
    if (city == "My Tho") return "Mỹ Tho";
    if (city == "Ca Mau") return "Cà Mau";
    if (city == "Soc Trang") return "Sóc Trăng";
    if (city == "Bac Lieu") return "Bạc Liêu";
    if (city == "Ben Tre") return "Bến Tre";
    if (city == "Tra Vinh") return "Trà Vinh";
    if (city == "Vinh Long") return "Vĩnh Long";
    if (city == "Sa Dec") return "Sa Đéc";
    if (city == "Chau Doc") return "Châu Đốc";
    if (city == "Ha Tien") return "Hà Tiên";
    if (city == "Tam Ky") return "Tam Kỳ";
    if (city == "Quang Ngai") return "Quảng Ngãi";
    if (city == "Dong Hoi") return "Đồng Hới";
    if (city == "Dong Ha") return "Đông Hà";
    if (city == "Thanh Hoa") return "Thanh Hóa";
    if (city == "Vinh") return "Vinh";
    if (city == "Ha Tinh") return "Hà Tĩnh";
    if (city == "Nam Dinh") return "Nam Định";
    if (city == "Ninh Binh") return "Ninh Bình";
    if (city == "Phu Ly") return "Phủ Lý";
    if (city == "Thai Binh") return "Thái Bình";
    if (city == "Hai Duong") return "Hải Dương";
    if (city == "Hung Yen") return "Hưng Yên";
    if (city == "Bac Ninh") return "Bắc Ninh";
    if (city == "Bac Giang") return "Bắc Giang";
    if (city == "Lang Son") return "Lạng Sơn";
    if (city == "Ha Long") return "Hạ Long";
    if (city == "Uong Bi") return "Uông Bí";
    if (city == "Cam Pha") return "Cẩm Phả";
    if (city == "Mong Cai") return "Móng Cái";
    if (city == "Viet Tri") return "Việt Trì";
    if (city == "Thai Nguyen") return "Thái Nguyên";
    if (city == "Tuyen Quang") return "Tuyên Quang";
    if (city == "Yen Bai") return "Yên Bái";
    if (city == "Lao Cai") return "Lào Cai";
    if (city == "Son La") return "Sơn La";
    if (city == "Dien Bien Phu") return "Điện Biên Phủ";
    if (city == "Hoa Binh") return "Hòa Bình";
    if (city == "Bac Kan") return "Bắc Kạn";
    if (city == "Cao Bang") return "Cao Bằng";
    if (city == "Ha Giang") return "Hà Giang";
    if (city == "Kon Tum") return "Kon Tum";
    if (city == "Gia Nghia") return "Gia Nghĩa";
    if (city == "Tuy Hoa") return "Tuy Hòa";
    if (city == "Phan Rang-Thap Cham") return "Phan Rang - Tháp Chàm";
    if (city == "Phan Rang") return "Phan Rang";
    if (city == "Thu Duc") return "Thủ Đức";

    // Provinces / alternate English spellings sometimes returned by IP geolocation
    if (city == "Ba Ria") return "Bà Rịa";
    if (city == "Ba Ria-Vung Tau") return "Bà Rịa - Vũng Tàu";
    if (city == "Quang Ninh") return "Quảng Ninh";
    if (city == "Quang Nam") return "Quảng Nam";
    if (city == "Quang Tri") return "Quảng Trị";
    if (city == "Quang Binh") return "Quảng Bình";
    if (city == "Thua Thien Hue") return "Thừa Thiên Huế";
    if (city == "Khanh Hoa") return "Khánh Hòa";
    if (city == "Binh Dinh") return "Bình Định";
    if (city == "Binh Thuan") return "Bình Thuận";
    if (city == "Ninh Thuan") return "Ninh Thuận";
    if (city == "Phu Yen") return "Phú Yên";
    if (city == "Dak Lak") return "Đắk Lắk";
    if (city == "Dak Nong") return "Đắk Nông";
    if (city == "Lam Dong") return "Lâm Đồng";
    if (city == "Gia Lai") return "Gia Lai";
    if (city == "Dong Nai") return "Đồng Nai";
    if (city == "Binh Duong") return "Bình Dương";
    if (city == "Binh Phuoc") return "Bình Phước";
    if (city == "Tay Ninh") return "Tây Ninh";
    if (city == "Long An") return "Long An";
    if (city == "Tien Giang") return "Tiền Giang";
    if (city == "Ben Tre Province") return "Bến Tre";
    if (city == "Tra Vinh Province") return "Trà Vinh";
    if (city == "Vinh Long Province") return "Vĩnh Long";
    if (city == "Dong Thap") return "Đồng Tháp";
    if (city == "An Giang") return "An Giang";
    if (city == "Kien Giang") return "Kiên Giang";
    if (city == "Hau Giang") return "Hậu Giang";
    if (city == "Soc Trang Province") return "Sóc Trăng";
    if (city == "Bac Lieu Province") return "Bạc Liêu";
    if (city == "Ca Mau Province") return "Cà Mau";
    if (city == "Ha Nam") return "Hà Nam";
    if (city == "Hung Yen Province") return "Hưng Yên";
    if (city == "Thai Binh Province") return "Thái Bình";
    if (city == "Nam Dinh Province") return "Nam Định";
    if (city == "Ninh Binh Province") return "Ninh Bình";
    if (city == "Phu Tho") return "Phú Thọ";
    if (city == "Bac Giang Province") return "Bắc Giang";
    if (city == "Bac Ninh Province") return "Bắc Ninh";
    if (city == "Vinh Phuc") return "Vĩnh Phúc";
    if (city == "Hoa Binh Province") return "Hòa Bình";
    if (city == "Thai Nguyen Province") return "Thái Nguyên";
    if (city == "Tuyen Quang Province") return "Tuyên Quang";
    if (city == "Yen Bai Province") return "Yên Bái";
    if (city == "Lao Cai Province") return "Lào Cai";
    if (city == "Son La Province") return "Sơn La";
    if (city == "Dien Bien") return "Điện Biên";
    if (city == "Lai Chau") return "Lai Châu";
    if (city == "Bac Kan Province") return "Bắc Kạn";
    if (city == "Cao Bang Province") return "Cao Bằng";
    if (city == "Ha Giang Province") return "Hà Giang";
    if (city == "Lang Son Province") return "Lạng Sơn";
    if (city == "Thanh Hoa Province") return "Thanh Hóa";
    if (city == "Nghe An") return "Nghệ An";
    if (city == "Ha Tinh Province") return "Hà Tĩnh";

    return city;
};
        location_name = localize_city(geo.city);
        latitude = geo.latitude;
        longitude = geo.longitude;
    } else {
        ESP_LOGW(TAG, "FetchCurrent: using fallback config location");
    }

    char url[512];
    snprintf(
        url,
        sizeof(url),
        "https://api.openweathermap.org/data/3.0/onecall?lat=%s&lon=%s&exclude=minutely,hourly,daily,alerts&units=metric&lang=%s&appid=%s",
        latitude.c_str(),
        longitude.c_str(),
        CONFIG_OPENWEATHER_LANG,
        CONFIG_OPENWEATHER_API_KEY
    );

    std::string response;
    if (!HttpGet(url, response)) {
        return false;
    }

    return ParseOneCallResponse(response, location_name, out_info);
}
bool WeatherService::ParseGeoLocationResponse(const std::string& response, GeoLocationInfo& out_geo) {
    cJSON* root = cJSON_Parse(response.c_str());
    if (root == nullptr) {
        ESP_LOGE(TAG, "ParseGeoLocationResponse: cJSON_Parse failed");
        return false;
    }

    cJSON* success = cJSON_GetObjectItemCaseSensitive(root, "success");
    cJSON* city = cJSON_GetObjectItemCaseSensitive(root, "city");
    cJSON* latitude = cJSON_GetObjectItemCaseSensitive(root, "latitude");
    cJSON* longitude = cJSON_GetObjectItemCaseSensitive(root, "longitude");

    if (!cJSON_IsBool(success) || !cJSON_IsTrue(success) ||
        !cJSON_IsString(city) || !cJSON_IsNumber(latitude) || !cJSON_IsNumber(longitude)) {
        cJSON_Delete(root);
        return false;
    }

    char lat_text[32];
    char lon_text[32];
    snprintf(lat_text, sizeof(lat_text), "%.6f", latitude->valuedouble);
    snprintf(lon_text, sizeof(lon_text), "%.6f", longitude->valuedouble);

    out_geo.city = city->valuestring ? city->valuestring : "";
    out_geo.latitude = lat_text;
    out_geo.longitude = lon_text;
    out_geo.valid = !out_geo.city.empty();

    cJSON_Delete(root);
    return out_geo.valid;
}
bool WeatherService::FetchGeoLocation(GeoLocationInfo& out_geo) {
    std::string response;
    if (!HttpGet("https://ipwho.is/", response)) {
        ESP_LOGW(TAG, "FetchGeoLocation: HttpGet failed");
        return false;
    }

    if (!ParseGeoLocationResponse(response, out_geo)) {
        ESP_LOGW(TAG, "FetchGeoLocation: ParseGeoLocationResponse failed");
        return false;
    }

    ESP_LOGI(TAG, "Geo location: city=%s lat=%s lon=%s",
        out_geo.city.c_str(),
        out_geo.latitude.c_str(),
        out_geo.longitude.c_str());

    return true;
}