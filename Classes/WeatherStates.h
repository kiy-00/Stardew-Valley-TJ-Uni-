#pragma once
#ifndef __WEATHER_STATES_H__
#define __WEATHER_STATES_H__
#include "IWeatherState.h"
#include "Enums.h"

//晴天
class SunnyWeatherState : public IWeatherState {
public:
    void update(WeatherSystem* weatherSystem) override;
    const WeatherType getWeatherName() const override { return WeatherType::SUNNY; }
};

//雨天
class RainyWeatherState : public IWeatherState {
public:
    void update(WeatherSystem* weatherSystem) override;
    const WeatherType getWeatherName() const override { return WeatherType::RAINY; }


};

//风暴
class StormWeatherState : public IWeatherState {
public:
    void update(WeatherSystem* weatherSystem) override;
    const WeatherType getWeatherName() const override { return WeatherType::STORM; }
};

//雪天
class SnowyWeatherState : public IWeatherState {
public:
    void update(WeatherSystem* weatherSystem) override;
    const WeatherType getWeatherName() const override { return WeatherType::SNOWY; }
};
#endif // !__WEATHER_STATES__
