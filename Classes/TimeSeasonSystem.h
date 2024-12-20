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
    // 季节枚举
    enum class Season {
        SPRING,
        SUMMER,
        FALL,
        WINTER
    };

    // 时间相关的结构体
    struct GameTime {
        int year;       // 游戏年份
        int season;     // 季节(0-3)
        int day;        // 当前日期(1-28)
        int hour;       // 小时(0-23)
        int minute;     // 分钟(0-59)

        bool operator==(const GameTime& other) const {
            return year == other.year && season == other.season &&
                day == other.day && hour == other.hour && minute == other.minute;
        }
    };

    // 事件数据结构
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

    // 创建和初始化
    static TimeSeasonSystem* getInstance();
    virtual bool init() override;

    // 时间控制
    void startTime();
    void pauseTime();
    void resumeTime();

    // 时间和季节获取
    const GameTime& getCurrentTime() const { return currentTime; }
    Season getCurrentSeason() const { return static_cast<Season>(currentTime.season); }
    std::string getCurrentSeasonString() const;
    int getCurrentDay() const { return currentTime.day; }
    int getCurrentYear() const { return currentTime.year; }

    // 时间设置
    void setTimeScale(float scale); // 设置时间流速
    void setTime(const GameTime& time); // 直接设置时间

    // 事件监听注册
    using SeasonChangeCallback = std::function<void(const SeasonChangeEvent&)>;
    using DayChangeCallback = std::function<void(const DayChangeEvent&)>;

    void addSeasonChangeListener(const std::string& name, const SeasonChangeCallback& callback);
    void addDayChangeListener(const std::string& name, const DayChangeCallback& callback);
    void removeSeasonChangeListener(const std::string& name);
    void removeDayChangeListener(const std::string& name);

    // 存档相关
    void saveToUserDefault();
    void loadFromUserDefault();

    // 常量
    static const int DAYS_PER_SEASON = 28;
    static const int HOURS_PER_DAY = 24;
    static const int MINUTES_PER_HOUR = 60;
    static const std::vector<std::string> SEASON_NAMES;

protected:
    TimeSeasonSystem();
    virtual ~TimeSeasonSystem();

private:
    static TimeSeasonSystem* instance;

    // 核心数据
    GameTime currentTime;
    float timeScale;
    float accumulatedTime;
    bool isRunning;

    // 回调存储
    std::map<std::string, SeasonChangeCallback> seasonChangeCallbacks;
    std::map<std::string, DayChangeCallback> dayChangeCallbacks;

    // 更新方法
    void update(float dt) override;
    void updateTime(float deltaMinutes);

    // 事件通知
    void notifySeasonChange(Season previousSeason);
    void notifyDayChange(int previousDay);

    // 时间计算辅助方法
    void advanceDay();
    void advanceSeason();
    void checkAndNotifyChanges(const GameTime& previousTime);

    // 辅助方法
    static Season intToSeason(int seasonInt);
    static std::string seasonToString(Season season);
};

#endif // __TIME_SEASON_SYSTEM_H__