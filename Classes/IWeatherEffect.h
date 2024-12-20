#pragma once
#ifndef __I_WEATHER_EFFECT_H__
#define __I_WEATHER_EFFECT_H__

#include "WeatherEffectFactory.h"
#include "Farmland.h"
#include "LandStates.h"
class WeatherEffect {
public:
    // 重载 apply 方法，根据土地状态执行不同效果
    virtual void applyToNormal(Farmland* land) = 0;
    virtual void applyToTilled(Farmland* land) = 0;
    virtual void applyToWatered(Farmland* land) = 0;
    virtual void applyToFertilized(Farmland* land) = 0;
    virtual void applyToWateredAndFertilized(Farmland* land) = 0;
    virtual void applyToCrop(Crop* crop) = 0;
    // 可以继续添加其他状态的处理方法
    virtual ~WeatherEffect() = default;
};

#endif // !__WEATHER_EFFECT_H__
