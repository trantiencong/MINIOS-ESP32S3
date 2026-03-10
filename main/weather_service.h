#ifndef _WEATHER_SERVICE_H_
#define _WEATHER_SERVICE_H_

#include <string>
#include <cstdint>

struct WeatherInfo {
    std::string location;
    std::string description;
    std::string temperature;
    std::string humidity;
    std::string icon_code;
    int64_t fetched_at_ms = 0;
    bool valid = false;
};

struct GeoLocationInfo {
    std::string city;
    std::string latitude;
    std::string longitude;
    bool valid = false;
};

class WeatherService {
public:
    bool FetchCurrent(WeatherInfo& out_info);

private:
    bool HttpGet(const std::string& url, std::string& response);
    bool ParseGeoLocationResponse(const std::string& response, GeoLocationInfo& out_geo);
    bool ParseOneCallResponse(
        const std::string& response,
        const std::string& location_name,
        WeatherInfo& out_info
    );
    bool FetchGeoLocation(GeoLocationInfo& out_geo);
};

#endif