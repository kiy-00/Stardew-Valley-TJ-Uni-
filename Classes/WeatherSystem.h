#ifndef __WEATHER_SYSTEM_H__
#define __WEATHER_SYSTEM_H__

#include "cocos2d.h"
#include <string>
#include <vector>
#include <map>
#include <functional>

USING_NS_CC;

// Weather types enum
enum class WeatherType {
    SUNNY = 0,
    CLOUDY,
    RAINY,
    SNOWY,
    FOGGY
    // 移除了 STORMY
};

// Weather change event structure
struct WeatherChangeEvent {
    WeatherType previousWeather;
    WeatherType newWeather;
    int year;
    int season;
    int day;
};

// Weather callback type definition
using WeatherChangeCallback = std::function<void(const WeatherChangeEvent&)>;

class WeatherSystem : public cocos2d::Node {
public:
    // Singleton methods
    static WeatherSystem* getInstance();
    virtual bool init() override;

    // Weather control
    void startWeatherSystem();
    void pauseWeatherSystem();
    void resumeWeatherSystem();

    // Weather state
    WeatherType getCurrentWeather() const { return currentWeather; }
    std::string getCurrentWeatherString() const;
    void setWeather(WeatherType weather);

    // Weather change probability
    void setWeatherChangeProbability(float probability);

    // Weather duration control
    void setMinWeatherDuration(float minutes) { minWeatherDuration = minutes; }
    void setMaxWeatherDuration(float minutes) { maxWeatherDuration = minutes; }

    // Callback management
    void addWeatherChangeListener(const std::string& name, const WeatherChangeCallback& callback);
    void removeWeatherChangeListener(const std::string& name);
    void setWeatherChangedCallback(const std::function<void(const std::string&)>& callback) {
        weatherChangedCallback = callback;
    }

    // Save/Load
    void saveToUserDefault();
    void loadFromUserDefault();

    // Season-specific weather probability adjustment
    void updateSeasonWeatherProbabilities(const std::string& season);

    static std::string weatherToString(WeatherType weather);

    void setWeatherDurationRange(float minDuration, float maxDuration);

protected:
    WeatherSystem();
    virtual ~WeatherSystem();

    // Update method
    virtual void update(float dt) override;

private:
    static WeatherSystem* instance;
    static const std::vector<std::string> WEATHER_NAMES;

    WeatherType currentWeather;
    float weatherChangeProbability;
    float minWeatherDuration;
    float maxWeatherDuration;
    float currentWeatherDuration;
    float elapsedTime;
    bool isRunning;

    std::map<std::string, WeatherChangeCallback> weatherChangeCallbacks;
    std::function<void(const std::string&)> weatherChangedCallback;

    // Weather probability maps for each season
    std::map<WeatherType, float> springProbabilities;
    std::map<WeatherType, float> summerProbabilities;
    std::map<WeatherType, float> fallProbabilities;
    std::map<WeatherType, float> winterProbabilities;

    void initializeSeasonProbabilities();
    void notifyWeatherChange(WeatherType previousWeather);
    WeatherType determineNextWeather(const std::map<WeatherType, float>& probabilities);
   

    CREATE_FUNC(WeatherSystem);
};

#endif // __WEATHER_SYSTEM_H__