// TimeSeasonSystem.h
#ifndef __TIME_SEASON_SYSTEM_H__
#define __TIME_SEASON_SYSTEM_H__

#include "cocos2d.h"
#include <functional>
#include <string>
#include <map>
#include <vector>

USING_NS_CC;

class TimeSeasonSystem : public Node {
public:
    // Season enumeration
    enum class Season {
        SPRING,
        SUMMER,
        FALL,
        WINTER
    };


    // Game time structure
    struct GameTime {
        int year;       // Game year
        int season;     // Season (0-3)
        int day;        // Current day (1-28)
        int hour;       // Hour (0-23)
        int minute;     // Minute (0-59)

        bool operator==(const GameTime& other) const {
            return year == other.year && season == other.season &&
                day == other.day && hour == other.hour && minute == other.minute;
        }
    };

    // Event structures
    struct SeasonChangeEvent {
        Season previousSeason;
        Season newSeason;
        int year;
        int day;
    };

    struct DayChangeEvent {
        int previousDay;
        int newDay;
        Season season;
        int year;
    };

    // Conversion functions
    static Season intToSeason(int seasonInt);
    static std::string seasonToString(Season season);

    // Singleton initialization
    static TimeSeasonSystem* getInstance();
    virtual bool init() override;

    // Time control
    void startTime();
    void pauseTime();
    void resumeTime();

    // Time getters
    const GameTime& getCurrentTime() const { return currentTime; }
    Season getCurrentSeason() const { return static_cast<Season>(currentTime.season); }
    std::string getCurrentSeasonString() const;
    int getCurrentDay() const { return currentTime.day; }
    int getCurrentYear() const { return currentTime.year; }

    // Time setters
   
  
    // Event listeners
    using SeasonChangeCallback = std::function<void(const SeasonChangeEvent&)>;
    using DayChangeCallback = std::function<void(const DayChangeEvent&)>;

    void addSeasonChangeListener(const std::string& name, const SeasonChangeCallback& callback);
    void addDayChangeListener(const std::string& name, const DayChangeCallback& callback);
    void removeSeasonChangeListener(const std::string& name);
    void removeDayChangeListener(const std::string& name);

    // Save and load
    void saveToUserDefault();
    void loadFromUserDefault();

    // 使用新的常量命名空间
    using Constants = GameTime;

    const float REAL_SECONDS_PER_GAME_MINUTE = 0.1f;  // Changed: 1 real second = 10 game minutes
    const float MINUTES_PER_SEASON = 60.0f;           // 60 game minutes = 1 season

    void setSeasonChangedCallback(const std::function<void(const std::string&)>& callback);

    // 添加时间变化回调的类型定义和设置函数
    using TimeChangedCallback = std::function<void(const GameTime&)>;
    void setTimeChangedCallback(const TimeChangedCallback& callback) {
        timeChangedCallback = callback;
    }

protected:
    TimeSeasonSystem();
    virtual ~TimeSeasonSystem();

private:
    static TimeSeasonSystem* instance;

    static const std::vector<std::string> SEASON_NAMES;

    // Time variables
    GameTime currentTime;
    float timeScale;
    float accumulatedTime;
    bool isRunning;

    // Callback storage
    std::map<std::string, SeasonChangeCallback> seasonChangeCallbacks;
    std::map<std::string, DayChangeCallback> dayChangeCallbacks;

    std::function<void(const std::string&)> seasonChangedCallback;

    // Update functions
    void update(float dt) override;
    void updateTime(float deltaMinutes);

    // 添加新的辅助方法
    void updateTimeByMinutes(int deltaMinutes);
    bool isValidTime(const GameTime& time) const;

    // Event notifications
    void notifySeasonChange(Season previousSeason);
    void notifyDayChange(int previousDay);

    // Time advancement functions
    void advanceDay();
    void advanceSeason();
    void checkAndNotifyChanges(const GameTime& previousTime);

    // 已有的成员...
    TimeChangedCallback timeChangedCallback;  // 添加回调成员变量
    
};

#endif // __TIME_SEASON_SYSTEM_H__