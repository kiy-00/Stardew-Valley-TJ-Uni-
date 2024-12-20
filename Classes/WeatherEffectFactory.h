#pragma once
#ifndef __WEATHER_EFFECT_FACTORY_H__
#define __WEATHER_EFFECT_FACTORY_H__
#include <memory>
#include <unordered_map>
#include <functional>
#include "Enums.h"

class WeatherEffect;
class SunnyEffect;
class StormEffect;
class RainyEffect;
class SnowyEffect;

class WeatherEffectFactory {
public:
    static std::unique_ptr<WeatherEffect> createEffect(WeatherType type) {
        static std::unordered_map<WeatherType, std::function<std::unique_ptr<WeatherEffect>()>> effectCreators = {
            {WeatherType::SUNNY, []() { return std::make_unique<SunnyEffect>(); }},
            {WeatherType::STORM, []() { return std::make_unique<StormEffect>(); }},
            {WeatherType::RAINY, []() { return std::make_unique<RainyEffect>(); }},
            {WeatherType::SNOWY, []() { return std::make_unique<SnowyEffect>(); }}
        };

        auto it = effectCreators.find(type);
        return it != effectCreators.end() ? it->second() : nullptr;
    }
};
#endif // !__WEATHER_EFFECT_FACTORY_H__

