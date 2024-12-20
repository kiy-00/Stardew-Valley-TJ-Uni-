// TimeSeasonSystem.cpp
#include "TimeSeasonSystem.h"

// 静态成员初始化
TimeSeasonSystem* TimeSeasonSystem::instance = nullptr;
const std::vector<std::string> TimeSeasonSystem::SEASON_NAMES = { "spring", "summer", "fall", "winter" };

// 构造函数
TimeSeasonSystem::TimeSeasonSystem()
    : currentTime{ 1, 0, 1, 6, 0 }  // 从第一年春季第一天早上6点开始
    , timeScale(1.0f)
    , accumulatedTime(0.0f)
    , isRunning(false) {
}

// 析构函数
TimeSeasonSystem::~TimeSeasonSystem() {
    instance = nullptr;
}

// 单例获取
TimeSeasonSystem* TimeSeasonSystem::getInstance() {
    if (!instance) {
        instance = new (std::nothrow) TimeSeasonSystem();
        if (instance && instance->init()) {
            instance->autorelease();
        }
        else {
            CC_SAFE_DELETE(instance);
        }
    }
    return instance;
}

// 初始化
bool TimeSeasonSystem::init() {
    if (!Node::init()) {
        return false;
    }

    loadFromUserDefault();

    // 启动更新
    this->scheduleUpdate();

    return true;
}

// 时间控制方法
void TimeSeasonSystem::startTime() {
    isRunning = true;
    this->resume();
}

void TimeSeasonSystem::pauseTime() {
    isRunning = false;
    this->pause();
}

void TimeSeasonSystem::resumeTime() {
    isRunning = true;
    this->resume();
}

// 时间更新
void TimeSeasonSystem::update(float dt) {
    if (!isRunning) return;

    // 计算时间流逝
    // 假设1秒真实时间 = 1分钟游戏时间 * timeScale
    float deltaMinutes = dt * 60.0f * timeScale;
    updateTime(deltaMinutes);
}

void TimeSeasonSystem::updateTime(float deltaMinutes) {
    GameTime previousTime = currentTime;

    // 累加分钟并处理进位
    accumulatedTime += deltaMinutes;
    while (accumulatedTime >= 1.0f) {
        currentTime.minute++;
        accumulatedTime -= 1.0f;

        if (currentTime.minute >= MINUTES_PER_HOUR) {
            currentTime.minute = 0;
            currentTime.hour++;

            if (currentTime.hour >= HOURS_PER_DAY) {
                currentTime.hour = 0;
                advanceDay();
            }
        }
    }

    // 检查并触发事件
    checkAndNotifyChanges(previousTime);
}

void TimeSeasonSystem::advanceDay() {
    int previousDay = currentTime.day;
    currentTime.day++;

    if (currentTime.day > DAYS_PER_SEASON) {
        currentTime.day = 1;
        advanceSeason();
    }

    notifyDayChange(previousDay);
}

void TimeSeasonSystem::advanceSeason() {
    Season previousSeason = static_cast<Season>(currentTime.season);
    currentTime.season = (currentTime.season + 1) % 4;

    if (currentTime.season == 0) {  // 新的一年
        currentTime.year++;
    }

    notifySeasonChange(previousSeason);
}

// 事件通知
void TimeSeasonSystem::notifySeasonChange(Season previousSeason) {
    SeasonChangeEvent event{
        previousSeason,
        static_cast<Season>(currentTime.season),
        currentTime.year,
        currentTime.day
    };

    for (const auto& callback : seasonChangeCallbacks) {
        callback.second(event);
    }

    // 保存状态
    saveToUserDefault();
}

void TimeSeasonSystem::notifyDayChange(int previousDay) {
    DayChangeEvent event{
        previousDay,
        currentTime.day,
        static_cast<Season>(currentTime.season),
        currentTime.year
    };

    for (const auto& callback : dayChangeCallbacks) {
        callback.second(event);
    }
}

// 事件监听注册
void TimeSeasonSystem::addSeasonChangeListener(const std::string& name, const SeasonChangeCallback& callback) {
    seasonChangeCallbacks[name] = callback;
}

void TimeSeasonSystem::addDayChangeListener(const std::string& name, const DayChangeCallback& callback) {
    dayChangeCallbacks[name] = callback;
}

void TimeSeasonSystem::removeSeasonChangeListener(const std::string& name) {
    seasonChangeCallbacks.erase(name);
}

void TimeSeasonSystem::removeDayChangeListener(const std::string& name) {
    dayChangeCallbacks.erase(name);
}

// 时间设置
void TimeSeasonSystem::setTimeScale(float scale) {
    timeScale = std::max(0.0f, scale);  // 防止负数时间流速
}

void TimeSeasonSystem::setTime(const GameTime& time) {
    GameTime previousTime = currentTime;
    currentTime = time;

    // 确保数值在有效范围内
    currentTime.minute = std::clamp(currentTime.minute, 0, MINUTES_PER_HOUR - 1);
    currentTime.hour = std::clamp(currentTime.hour, 0, HOURS_PER_DAY - 1);
    currentTime.day = std::clamp(currentTime.day, 1, DAYS_PER_SEASON);
    currentTime.season = std::clamp(currentTime.season, 0, 3);
    currentTime.year = std::max(1, currentTime.year);

    // 检查并触发相应事件
    checkAndNotifyChanges(previousTime);
}

// 存档相关
void TimeSeasonSystem::saveToUserDefault() {
    auto ud = UserDefault::getInstance();
    ud->setIntegerForKey("game_year", currentTime.year);
    ud->setIntegerForKey("game_season", currentTime.season);
    ud->setIntegerForKey("game_day", currentTime.day);
    ud->setIntegerForKey("game_hour", currentTime.hour);
    ud->setIntegerForKey("game_minute", currentTime.minute);
    ud->setFloatForKey("game_time_scale", timeScale);
    ud->flush();
}

void TimeSeasonSystem::loadFromUserDefault() {
    auto ud = UserDefault::getInstance();
    currentTime.year = ud->getIntegerForKey("game_year", 1);
    currentTime.season = ud->getIntegerForKey("game_season", 0);
    currentTime.day = ud->getIntegerForKey("game_day", 1);
    currentTime.hour = ud->getIntegerForKey("game_hour", 6);
    currentTime.minute = ud->getIntegerForKey("game_minute", 0);
    timeScale = ud->getFloatForKey("game_time_scale", 1.0f);
}

// 辅助方法
void TimeSeasonSystem::checkAndNotifyChanges(const GameTime& previousTime) {
    if (currentTime.season != previousTime.season) {
        notifySeasonChange(static_cast<Season>(previousTime.season));
    }
    else if (currentTime.day != previousTime.day) {
        notifyDayChange(previousTime.day);
    }
}

TimeSeasonSystem::Season TimeSeasonSystem::intToSeason(int seasonInt) {
    return static_cast<Season>(seasonInt % 4);
}

std::string TimeSeasonSystem::seasonToString(Season season) {
    return SEASON_NAMES[static_cast<int>(season)];
}

std::string TimeSeasonSystem::getCurrentSeasonString() const {
    return SEASON_NAMES[currentTime.season];
}