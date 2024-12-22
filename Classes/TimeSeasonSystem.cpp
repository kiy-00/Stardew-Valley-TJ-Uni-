// TimeSeasonSystem.cpp
#include "TimeSeasonSystem.h"
#include "TimeConstants.h"

using namespace GameTime;
// Initialize static members
TimeSeasonSystem* TimeSeasonSystem::instance = nullptr;
const std::vector<std::string> TimeSeasonSystem::SEASON_NAMES = { "spring", "summer", "fall", "winter" };

// Constructor
TimeSeasonSystem::TimeSeasonSystem()
    : currentTime{ DEFAULT_YEAR, DEFAULT_SEASON, 
                  DEFAULT_DAY, DEFAULT_HOUR, 
                  DEFAULT_MINUTE }
    , timeScale(DEFAULT_TIME_SCALE)
    , isRunning(false) {
    CCLOG("TimeSeasonSystem constructed with: Year %d, Season %d, Day %d, %02d:%02d",
        currentTime.year, currentTime.season, currentTime.day, 
        currentTime.hour, currentTime.minute);
}

// Destructor
TimeSeasonSystem::~TimeSeasonSystem() {
    instance = nullptr;
}

// Singleton instance getter
TimeSeasonSystem* TimeSeasonSystem::getInstance() {
    if (!instance) {
        instance = new (std::nothrow) TimeSeasonSystem();
        if (instance && instance->init()) {
            instance->autorelease();
            // Ensure added to the current running scene
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

// Initialization
bool TimeSeasonSystem::init() {
    if (!Node::init()) {
        return false;
    }

    loadFromUserDefault();
    //CCLOG("TimeSeasonSystem initialized");
    scheduleUpdate(); // Ensure update() is called every frame
    return true;
}

// Start time progression
void TimeSeasonSystem::startTime() {
    //CCLOG("TimeSystem starting - timeScale: %.2f", timeScale);
    isRunning = true;
    scheduleUpdate(); 
    this->schedule(CC_SCHEDULE_SELECTOR(TimeSeasonSystem::update), 1.0f / 60.0f);  // 60fps
}

void TimeSeasonSystem::pauseTime() {
    //CCLOG("TimeSystem paused");
    isRunning = false;
    this->unschedule(CC_SCHEDULE_SELECTOR(TimeSeasonSystem::update));
}

void TimeSeasonSystem::resumeTime() {
    //CCLOG("TimeSystem resumed");
    startTime();
}

// Time update
void TimeSeasonSystem::update(float dt) {
    if (!isRunning) return;

    // 计算这一帧经过的游戏分钟数
    double minutesElapsed = dt * 60.0 * timeScale;
    int deltaMinutes = static_cast<int>(std::floor(minutesElapsed));
    
    if (deltaMinutes <= 0) return;
    
    updateTimeByMinutes(deltaMinutes);
}

void TimeSeasonSystem::updateTimeByMinutes(int deltaMinutes) {
    GameTime newTime = currentTime;
    
    // 计算总分钟数
    int totalMinutes = currentTime.minute + deltaMinutes;
    
    // 更新小时
    newTime.hour += totalMinutes / MINUTES_PER_HOUR;
    newTime.minute = totalMinutes % MINUTES_PER_HOUR;
    
    // 更新天数
    int totalHours = newTime.hour;
    newTime.day += totalHours / HOURS_PER_DAY;
    newTime.hour = totalHours % HOURS_PER_DAY;
    
    // 更新季节
    int totalDays = newTime.day - 1; // 因为day从1开始
    newTime.season += totalDays / DAYS_PER_SEASON;
    newTime.day = totalDays % DAYS_PER_SEASON + 1;
    
    // 更新年份
    newTime.year += newTime.season / SEASONS_PER_YEAR;
    newTime.season = newTime.season % SEASONS_PER_YEAR;
    
    // 检查时间变化并触发事件
    if (newTime.day != currentTime.day) {
        notifyDayChange(currentTime.day);
    }
    
    if (newTime.season != currentTime.season) {
        notifySeasonChange(static_cast<Season>(currentTime.season));
        if (seasonChangedCallback) {
            seasonChangedCallback(SEASON_NAMES[newTime.season]);
        }
    }

    // 检查小时是否发生变化
    if (newTime.hour != currentTime.hour && timeChangedCallback) {
      
        timeChangedCallback(newTime);
    }
    
    currentTime = newTime;
    saveToUserDefault();
}

bool TimeSeasonSystem::isValidTime(const GameTime& time) const {
    return time.minute >= 0 && time.minute < MINUTES_PER_HOUR
        && time.hour >= 0 && time.hour < HOURS_PER_DAY
        && time.day >= 1 && time.day <= DAYS_PER_SEASON
        && time.season >= 0 && time.season < SEASONS_PER_YEAR
        && time.year >= 1;
}

void TimeSeasonSystem::advanceDay() {
    int previousDay = currentTime.day;
    currentTime.day++;

    //CCLOG("Day advanced: %d -> %d", previousDay, currentTime.day);

    if (currentTime.day > DAYS_PER_SEASON) {
        currentTime.day = 1;
        advanceSeason();
    }

    notifyDayChange(previousDay);
    saveToUserDefault();
}

void TimeSeasonSystem::advanceSeason() {
    Season previousSeason = static_cast<Season>(currentTime.season);
    currentTime.season = (currentTime.season + 1) % 4;

    /*
    CCLOG("Season advanced: %s -> %s",
        seasonToString(previousSeason).c_str(),  // Use safe method
        getCurrentSeasonString().c_str());       // Use safe method

    */
    if (currentTime.season == 0) {  // New year
        currentTime.year++;
        //CCLOG("Year advanced to: %d", currentTime.year);
    }

    notifySeasonChange(previousSeason);
    saveToUserDefault();
}

// Event notifications
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

// Event listener registration
void TimeSeasonSystem::addSeasonChangeListener(const std::string& name, const SeasonChangeCallback& callback) {
    seasonChangeCallbacks[name] = callback;
    //CCLOG("Added season change listener: %s", name.c_str());
}

void TimeSeasonSystem::addDayChangeListener(const std::string& name, const DayChangeCallback& callback) {
    dayChangeCallbacks[name] = callback;
    //CCLOG("Added day change listener: %s", name.c_str());
}

void TimeSeasonSystem::removeSeasonChangeListener(const std::string& name) {
    seasonChangeCallbacks.erase(name);
}

void TimeSeasonSystem::removeDayChangeListener(const std::string& name) {
    dayChangeCallbacks.erase(name);
}

// Time scaling
//void TimeSeasonSystem::setTimeScale(float scale) {
//    timeScale = std::max(0.0f, scale);
//    //CCLOG("Time scale set to: %.2f", timeScale);
//}
//
//void TimeSeasonSystem::setTime(const GameTime& time) {
//    GameTime previousTime = currentTime;
//    
//    // Ensure all values are within valid ranges
//    currentTime.minute = std::clamp(time.minute, 0, MINUTES_PER_HOUR - 1);
//    currentTime.hour = std::clamp(time.hour, 0, HOURS_PER_DAY - 1);
//    currentTime.day = std::clamp(time.day, 1, DAYS_PER_SEASON);  // Start from 1
//    currentTime.season = std::clamp(time.season, 0, 3);
//    currentTime.year = std::max(1, time.year);  // At least start from year 1
//
//    /*CCLOG("Time set to - Year: %d, Season: %d, Day: %d, %02d:%02d",
//        currentTime.year, currentTime.season, currentTime.day,
//        currentTime.hour, currentTime.minute);
//*/
//
//    checkAndNotifyChanges(previousTime);
//    saveToUserDefault();
//}

// Save to user defaults
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
    
    // Set default values, ensure they are valid
    currentTime.year = std::max(1, ud->getIntegerForKey("game_year", DEFAULT_YEAR));  
    currentTime.season = std::clamp(ud->getIntegerForKey("game_season", DEFAULT_SEASON), 0, 3);
    currentTime.day = std::clamp(ud->getIntegerForKey("game_day", DEFAULT_DAY), 1, DAYS_PER_SEASON);
    currentTime.hour = std::clamp(ud->getIntegerForKey("game_hour", DEFAULT_HOUR), 0, HOURS_PER_DAY - 1);
    currentTime.minute = std::clamp(ud->getIntegerForKey("game_minute", DEFAULT_MINUTE), 0, MINUTES_PER_HOUR - 1);
    timeScale = DEFAULT_TIME_SCALE;  // 直接使用常量定义的值,不从缓存加载

    /*CCLOG("Loaded time - Year: %d, Season: %d, Day: %d, %02d:%02d",
        currentTime.year, currentTime.season, currentTime.day,
        currentTime.hour, currentTime.minute);
        */
}

// Check and notify changes
void TimeSeasonSystem::checkAndNotifyChanges(const GameTime& previousTime) {
    if (currentTime.season != previousTime.season) {
        notifySeasonChange(static_cast<Season>(previousTime.season));
    }
    else if (currentTime.day != previousTime.day) {
        notifyDayChange(previousTime.day);
    }
}

std::string TimeSeasonSystem::seasonToString(Season season) {
    size_t index = static_cast<size_t>(season);  // Use size_t to avoid signed comparison warning
    if (index >= SEASON_NAMES.size()) {
        return "unknown";
    }
    return SEASON_NAMES[index];
}

std::string TimeSeasonSystem::getCurrentSeasonString() const {
    size_t index = static_cast<size_t>(currentTime.season);
    if (index >= SEASON_NAMES.size()) {
        //CCLOG("Invalid season index: %d", currentTime.season);
        return "spring";
    }
    return SEASON_NAMES[index];
}

void TimeSeasonSystem::setSeasonChangedCallback(const std::function<void(const std::string&)>& callback) {
    seasonChangedCallback = callback;
}