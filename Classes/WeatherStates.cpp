#include "WeatherStates.h"
#include "Farmland.h"


//晴天
void SunnyWeatherState::update(WeatherSystem* weatherSystem) {
    // 10% 概率变为雨天
    if (rand() % 100 < 10) {
        weatherSystem->changeWeather(new RainyWeatherState());
    }
    // 5% 概率变为风暴
    else if (rand() % 100 < 5) {
        weatherSystem->changeWeather(new StormWeatherState());
    }
}


//雨天
void RainyWeatherState::update(WeatherSystem* weatherSystem) {
    // 20% 概率变为晴天
    if (rand() % 100 < 20) {
        weatherSystem->changeWeather(new SunnyWeatherState());
    }
    // 10% 概率变为风暴
    else if (rand() % 100 < 10) {
        weatherSystem->changeWeather(new StormWeatherState());
    }
}


//风暴
// StormWeatherState.cpp
void StormWeatherState::update(WeatherSystem* weatherSystem) {
    // 30% 概率变为雨天
    if (rand() % 100 < 30) {
        weatherSystem->changeWeather(new RainyWeatherState());
    }
    // 5% 概率变为晴天
    else if (rand() % 100 < 5) {
        weatherSystem->changeWeather(new SunnyWeatherState());
    }
}


//雪天
void SnowyWeatherState::update(WeatherSystem* weatherSystem) {
    if (rand() % 100 < 50) {
        weatherSystem->changeWeather(new SunnyWeatherState());  // 50% 的概率变为晴天
    }
    else if (rand() % 100 < 30) {
        weatherSystem->changeWeather(new RainyWeatherState());  // 30% 的概率变为雨天
    }
    else {
        weatherSystem->changeWeather(new StormWeatherState());  // 20% 的概率变为风暴
    }
}
