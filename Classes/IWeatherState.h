// WeatherState.h
#pragma once
#ifndef __I_WEATHER_STATE_H__
#define __I_WEATHER_STATE_H__
#include "cocos2d.h"
#include "Enums.h"

class WeatherSystem;

class IWeatherState {
public:
    virtual ~IWeatherState() {}
    virtual void update(WeatherSystem* weatherSystem) = 0;
    virtual const WeatherType getWeatherName() const = 0;
};
#endif