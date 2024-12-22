#include "WeatherSystem.h"
#include "TimeSeasonSystem.h"
#include <random>

// Static member initialization
WeatherSystem* WeatherSystem::instance = nullptr;
const std::vector<std::string> WeatherSystem::WEATHER_NAMES = {
    "sunny", "cloudy", "rainy", "snowy", "foggy"  // 移除了 "stormy"
};

WeatherSystem::WeatherSystem()
    : currentWeather(WeatherType::SUNNY)
    , weatherChangeProbability(0.8f)        // 增加变化概率
    , minWeatherDuration(10.0f)             
    , maxWeatherDuration(20.0f)            
    , currentWeatherDuration(10.0f)        
    , elapsedTime(0.0f)
    , isRunning(false) {
    initializeSeasonProbabilities();

    // 随机持续时间代码（临时注释）
    // 移除这段代码，因为它会在构造函数中过早设置持续时间
    /* 删除:
    currentWeatherDuration = minWeatherDuration +
        static_cast<float>(rand()) / RAND_MAX * (maxWeatherDuration - minWeatherDuration);
    */
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
    
    // 初始化时立即通知当前天气，触发特效生成
    WeatherType previousWeather = currentWeather;
    notifyWeatherChange(previousWeather);
    if (weatherChangedCallback) {
        weatherChangedCallback(getCurrentWeatherString());
    }
    
    scheduleUpdate();
    return true;
}

void WeatherSystem::initializeSeasonProbabilities() {
    // Spring probabilities - 重新分配概率
    springProbabilities = {
        {WeatherType::SUNNY, 0.35f},
        {WeatherType::CLOUDY, 0.25f},
        {WeatherType::RAINY, 0.25f},
        {WeatherType::FOGGY, 0.15f},
        {WeatherType::SNOWY, 0.0f}
    };

    // Summer probabilities - 重新分配概率
    summerProbabilities = {
        {WeatherType::SUNNY, 0.45f},
        {WeatherType::CLOUDY, 0.25f},
        {WeatherType::RAINY, 0.25f},
        {WeatherType::FOGGY, 0.05f},
        {WeatherType::SNOWY, 0.0f}
    };

    // Fall probabilities - 重新分配概率
    fallProbabilities = {
        {WeatherType::SUNNY, 0.25f},
        {WeatherType::CLOUDY, 0.30f},
        {WeatherType::RAINY, 0.25f},
        {WeatherType::FOGGY, 0.20f},
        {WeatherType::SNOWY, 0.0f}
    };

    // Winter probabilities - 重新分配概率
    winterProbabilities = {
        {WeatherType::SUNNY, 0.20f},
        {WeatherType::CLOUDY, 0.25f},
        {WeatherType::RAINY, 0.10f},
        {WeatherType::FOGGY, 0.15f},
        {WeatherType::SNOWY, 0.30f}
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
    CCLOG("Weather System Status:");
    CCLOG("Current Weather: %s", getCurrentWeatherString().c_str());
    CCLOG("Elapsed Time: %.2f / %.2f", elapsedTime, currentWeatherDuration);
    
    if (elapsedTime >= currentWeatherDuration) {
        WeatherType previousWeather = currentWeather;
        
        // 获取当前季节对应的天气概率表
        std::map<WeatherType, float> currentProbabilities;
        auto timeSystem = dynamic_cast<TimeSeasonSystem*>(Director::getInstance()->getRunningScene()->getChildByName("TimeSeasonSystem"));
        if (timeSystem) {
            std::string season = timeSystem->getCurrentSeasonString();
            CCLOG("Current Season: %s", season.c_str());
            
            // 根据季节选择概率表
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
                currentProbabilities = springProbabilities;
            }
            
            // 更新当前季节的天气变化概率
            updateSeasonWeatherProbabilities(season);
        }
        else {
            CCLOG("Warning: TimeSystem not found, using spring probabilities");
            currentProbabilities = springProbabilities;
        }

        // 强制生成新天气
        WeatherType newWeather = determineNextWeather(currentProbabilities);
        while (newWeather == currentWeather) {
            newWeather = determineNextWeather(currentProbabilities);
        }
        
        // 设置新天气
        setWeather(newWeather);
        CCLOG("Weather Changed: %s -> %s", 
              weatherToString(previousWeather).c_str(),
              weatherToString(newWeather).c_str());
        
        // 重置计时和持续时间
        elapsedTime = 0.0f;
        currentWeatherDuration = minWeatherDuration +
            static_cast<float>(rand()) / RAND_MAX * (maxWeatherDuration - minWeatherDuration);
        CCLOG("New Weather Duration: %.2f", currentWeatherDuration);
    }
}

WeatherType WeatherSystem::determineNextWeather(const std::map<WeatherType, float>& probabilities) {
    float random = static_cast<float>(rand()) / RAND_MAX;
    float cumulativeProbability = 0.0f;
    
    CCLOG("Weather Selection - Random Value: %.2f", random);
    
    for (const auto& pair : probabilities) {
        cumulativeProbability += pair.second;
        CCLOG("Weather Option: %s, Probability: %.2f, Cumulative: %.2f",
              weatherToString(pair.first).c_str(), pair.second, cumulativeProbability);
        
        if (random <= cumulativeProbability) {
            CCLOG("Selected Weather: %s", weatherToString(pair.first).c_str());
            return pair.first;
        }
    }
    
    CCLOG("Fallback to Sunny Weather");
    return WeatherType::SUNNY;
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
    size_t index = static_cast<size_t>(weather);
    if (index >= WEATHER_NAMES.size()) {
        CCLOG("Warning: Invalid weather index: %zu", index);
        return "sunny";  // 默认返回
    }
    return WEATHER_NAMES[index];
}

std::string WeatherSystem::getCurrentWeatherString() const {
    std::string result = weatherToString(currentWeather);
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
    minWeatherDuration = ud->getFloatForKey("min_weather_duration", 10.0f);
    maxWeatherDuration = ud->getFloatForKey("max_weather_duration", 20.0f);
    
    // 确保在加载配置后设置新的持续时间
    currentWeatherDuration = minWeatherDuration +
        static_cast<float>(rand()) / RAND_MAX * (maxWeatherDuration - minWeatherDuration);
    
    // 重置经过的时间
    elapsedTime = 0.0f;
}

void WeatherSystem::setWeatherDurationRange(float minDuration, float maxDuration) {
    minWeatherDuration = std::max(0.0f, minDuration);
    maxWeatherDuration = std::max(minWeatherDuration, maxDuration);
    
    // 若需要立刻重置当前天气剩余持续时间，可在此处加入逻辑
    // ...existing code...
}