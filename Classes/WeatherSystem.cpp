#include "WeatherSystem.h"
#include "TimeSeasonSystem.h"
#include <random>

// Static member initialization
WeatherSystem* WeatherSystem::instance = nullptr;
const std::vector<std::string> WeatherSystem::WEATHER_NAMES = {
    "sunny", "cloudy", "rainy", "stormy", "snowy", "foggy"
};

WeatherSystem::WeatherSystem()
    : currentWeather(WeatherType::SUNNY)
    , weatherChangeProbability(0.2f)        // 增加变化概率
    , minWeatherDuration(10.0f)             // 改为10秒
    , maxWeatherDuration(10.0f)            // 改为10秒
    , currentWeatherDuration(10.0f)         // 改为10秒
    , elapsedTime(0.0f)
    , isRunning(false) {
    initializeSeasonProbabilities();

    // 随机持续时间代码（临时注释）
    /* currentWeatherDuration = minWeatherDuration +
        static_cast<float>(rand()) / RAND_MAX * (maxWeatherDuration - minWeatherDuration);*/
}

WeatherSystem::~WeatherSystem() {
    instance = nullptr;
}

WeatherSystem* WeatherSystem::getInstance() {
    if (!instance) {
        instance = new (std::nothrow) WeatherSystem();
        if (instance && instance->init()) {
            instance->autorelease();
            if (Director::getInstance()->getRunningScene()) {
                Director::getInstance()->getRunningScene()->addChild(instance);
            }
        }
        else {
            CC_SAFE_DELETE(instance);
        }
    }
    return instance;
}

bool WeatherSystem::init() {
    if (!Node::init()) {
        return false;
    }

    loadFromUserDefault();
    scheduleUpdate();
    return true;
}

void WeatherSystem::initializeSeasonProbabilities() {
    // Spring probabilities
    springProbabilities = {
        {WeatherType::SUNNY, 0.35f},
        {WeatherType::CLOUDY, 0.25f},
        {WeatherType::RAINY, 0.25f},
        {WeatherType::STORMY, 0.05f},
        {WeatherType::FOGGY, 0.10f},
        {WeatherType::SNOWY, 0.0f}
    };

    // Summer probabilities - increased chance of sunny and stormy weather
    summerProbabilities = {
        {WeatherType::SUNNY, 0.40f},
        {WeatherType::CLOUDY, 0.20f},
        {WeatherType::RAINY, 0.20f},
        {WeatherType::STORMY, 0.15f},
        {WeatherType::FOGGY, 0.05f},
        {WeatherType::SNOWY, 0.0f}
    };

    // Fall probabilities - more varied weather
    fallProbabilities = {
        {WeatherType::SUNNY, 0.25f},
        {WeatherType::CLOUDY, 0.30f},
        {WeatherType::RAINY, 0.25f},
        {WeatherType::STORMY, 0.05f},
        {WeatherType::FOGGY, 0.15f},
        {WeatherType::SNOWY, 0.0f}
    };

    // Winter probabilities - added snow chance
    winterProbabilities = {
        {WeatherType::SUNNY, 0.20f},
        {WeatherType::CLOUDY, 0.25f},
        {WeatherType::RAINY, 0.10f},
        {WeatherType::STORMY, 0.05f},
        {WeatherType::FOGGY, 0.15f},
        {WeatherType::SNOWY, 0.25f}
    };
}

void WeatherSystem::startWeatherSystem() {
    isRunning = true;
    scheduleUpdate();
}

void WeatherSystem::pauseWeatherSystem() {
    isRunning = false;
    unscheduleUpdate();
}

void WeatherSystem::resumeWeatherSystem() {
    startWeatherSystem();
}

void WeatherSystem::update(float dt) {
    if (!isRunning) return;

    elapsedTime += dt;

    // 添加调试日志
    if (elapsedTime >= currentWeatherDuration) {
        CCLOG("Weather Update - Current Duration: %.2f, Elapsed: %.2f",
            currentWeatherDuration, elapsedTime);
        // Time to change weather
        WeatherType previousWeather = currentWeather;

        // Get current season's probabilities
        std::map<WeatherType, float> currentProbabilities;
        auto timeSystem = dynamic_cast<TimeSeasonSystem*>(Director::getInstance()->getRunningScene()->getChildByName("TimeSeasonSystem"));
        if (timeSystem) {
            std::string season = timeSystem->getCurrentSeasonString();
            if (season == "spring") {
                currentProbabilities = springProbabilities;
            }
            else if (season == "summer") {
                currentProbabilities = summerProbabilities;
            }
            else if (season == "fall") {
                currentProbabilities = fallProbabilities;
            }
            else if (season == "winter") {
                currentProbabilities = winterProbabilities;
            }
            else {
                currentProbabilities = springProbabilities; // fallback
            }
            updateSeasonWeatherProbabilities(season);
        }
        else {
            currentProbabilities = springProbabilities; // fallback if no time system
        }

        // Determine and set new weather
        WeatherType newWeather = determineNextWeather(currentProbabilities);
        setWeather(newWeather);

        // Reset timers
        elapsedTime = 0.0f;
        
        // 测试阶段：使用固定的2秒持续时间
        currentWeatherDuration = 2.0f;
        
        // 随机持续时间代码（临时注释）
        /*
        currentWeatherDuration = minWeatherDuration +
            static_cast<float>(rand()) / RAND_MAX * (maxWeatherDuration - minWeatherDuration);
        */

        // Notify about weather change
        if (previousWeather != currentWeather) {
            notifyWeatherChange(previousWeather);
        }
    }
}

WeatherType WeatherSystem::determineNextWeather(const std::map<WeatherType, float>& probabilities) {
    float random = static_cast<float>(rand()) / RAND_MAX;
    float cumulativeProbability = 0.0f;

    for (const auto& pair : probabilities) {
        cumulativeProbability += pair.second;
        if (random <= cumulativeProbability) {
            return pair.first;
        }
    }

    return WeatherType::SUNNY; // Default fallback
}

void WeatherSystem::setWeather(WeatherType weather) {
    WeatherType previousWeather = currentWeather;
    currentWeather = weather;

    if (previousWeather != currentWeather) {
        notifyWeatherChange(previousWeather);
        if (weatherChangedCallback) {
            weatherChangedCallback(getCurrentWeatherString());
        }
        saveToUserDefault();
    }
}

void WeatherSystem::notifyWeatherChange(WeatherType previousWeather) {
    WeatherChangeEvent event{
        previousWeather,
        currentWeather,
        0, // These should be filled with actual values from TimeSeasonSystem
        0,
        0
    };

    for (const auto& callback : weatherChangeCallbacks) {
        callback.second(event);
    }
}

void WeatherSystem::updateSeasonWeatherProbabilities(const std::string& season) {
    if (season == "spring") {
        weatherChangeProbability = 0.15f;
    }
    else if (season == "summer") {
        weatherChangeProbability = 0.1f;
    }
    else if (season == "fall") {
        weatherChangeProbability = 0.2f;
    }
    else if (season == "winter") {
        weatherChangeProbability = 0.12f;
    }
}

void WeatherSystem::setWeatherChangeProbability(float probability) {
    weatherChangeProbability = std::clamp(probability, 0.0f, 1.0f);
}

void WeatherSystem::addWeatherChangeListener(const std::string& name, const WeatherChangeCallback& callback) {
    weatherChangeCallbacks[name] = callback;
}

void WeatherSystem::removeWeatherChangeListener(const std::string& name) {
    weatherChangeCallbacks.erase(name);
}

std::string WeatherSystem::weatherToString(WeatherType weather) {
    CCLOG("Converting weather type: %d", static_cast<int>(weather));  // 添加调试日志
    size_t index = static_cast<size_t>(weather);
    if (index >= WEATHER_NAMES.size()) {
        CCLOG("Warning: Invalid weather index: %zu", index);
        return "sunny";  // 默认返回
    }
    CCLOG("Weather name at index %zu: %s", index, WEATHER_NAMES[index].c_str());
    return WEATHER_NAMES[index];
}

std::string WeatherSystem::getCurrentWeatherString() const {
    std::string result = weatherToString(currentWeather);
    CCLOG("getCurrentWeatherString returning: %s", result.c_str());
    return result;
}

void WeatherSystem::saveToUserDefault() {
    auto ud = UserDefault::getInstance();
    ud->setIntegerForKey("weather_type", static_cast<int>(currentWeather));
    ud->setFloatForKey("weather_change_probability", weatherChangeProbability);
    ud->setFloatForKey("min_weather_duration", minWeatherDuration);
    ud->setFloatForKey("max_weather_duration", maxWeatherDuration);
    ud->flush();
}

void WeatherSystem::loadFromUserDefault() {
    auto ud = UserDefault::getInstance();
    currentWeather = static_cast<WeatherType>(ud->getIntegerForKey("weather_type", 0));
    weatherChangeProbability = ud->getFloatForKey("weather_change_probability", 0.1f);
    
    // 固定持续时间为10秒
    minWeatherDuration = 10.0f;
    maxWeatherDuration = 10.0f;
    currentWeatherDuration = 10.0f;
    
    // 随机持续时间代码（临时注释）
    /*
    currentWeatherDuration = minWeatherDuration +
        static_cast<float>(rand()) / RAND_MAX * (maxWeatherDuration - minWeatherDuration);
    */
}