#ifndef _WEATHER_SERVICE_H_
#define _WEATHER_SERVICE_H_

#include <string>
#include <cstdint>

struct WeatherInfo {
    std::string location;
    std::string description;
    std::string temperature;
    std::string icon_code;
    int64_t fetched_at_ms = 0;
    bool valid = false;
};

class WeatherService {
public:
    bool FetchCurrent(WeatherInfo& out_info);

private:
    bool HttpGet(const std::string& url, std::string& response);
    bool ParseOneCallResponse(const std::string& response, WeatherInfo& out_info);
};

#endif