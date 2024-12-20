#pragma once
#ifndef __I_WEATHER_OBSERVER__
#define __I_WEATHER_OBSERVER__
#include"Enums.h"

class IWeatherObserver {
public:
    virtual ~IWeatherObserver() = default;
    virtual void onWeatherChanged(const WeatherType weatherType) = 0;
};

#endif // !_I_WEATHER_OBSERVER_
