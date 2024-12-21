// TimeSeasonSystem.cpp
#include "TimeSeasonSystem.h"

// ��̬��Ա��ʼ��
TimeSeasonSystem* TimeSeasonSystem::instance = nullptr;
const std::vector<std::string> TimeSeasonSystem::SEASON_NAMES = { "spring", "summer", "fall", "winter" };

// ���캯��
TimeSeasonSystem::TimeSeasonSystem()
    : currentTime{ 2024, 0, 1, 0, 0 }  // 确保从第1天开始，而不是第0天
    , timeScale(67.2f)
    , isRunning(false) {
    /*CCLOG("TimeSeasonSystem constructed with: Year %d, Season %d, Day %d, %02d:%02d",
        currentTime.year, currentTime.season, currentTime.day, 
        currentTime.hour, currentTime.minute);
        */
}

// ��������
TimeSeasonSystem::~TimeSeasonSystem() {
    instance = nullptr;
}

// ������ȡ
TimeSeasonSystem* TimeSeasonSystem::getInstance() {
    if (!instance) {
        instance = new (std::nothrow) TimeSeasonSystem();
        if (instance && instance->init()) {
            instance->autorelease();
            // ȷ�����ӵ���ǰ���г���
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

// ��ʼ��
bool TimeSeasonSystem::init() {
    if (!Node::init()) {
        return false;
    }

    loadFromUserDefault();
    //CCLOG("TimeSeasonSystem initialized");
    scheduleUpdate(); // 确保每帧都会调用 update()
    return true;
}

// ʱ����Ʒ���
void TimeSeasonSystem::startTime() {
    CCLOG("TimeSystem starting - timeScale: %.2f", timeScale);
    isRunning = true;
    scheduleUpdate(); 
    this->schedule(CC_SCHEDULE_SELECTOR(TimeSeasonSystem::update), 1.0f / 60.0f);  // 60fps����
}

void TimeSeasonSystem::pauseTime() {
    CCLOG("TimeSystem paused");
    isRunning = false;
    this->unschedule(CC_SCHEDULE_SELECTOR(TimeSeasonSystem::update));
}

void TimeSeasonSystem::resumeTime() {
    //CCLOG("TimeSystem resumed");
    startTime();
}

// ʱ�����
void TimeSeasonSystem::update(float dt) {
    if (!isRunning) {
        //CCLOG("TimeSystem is not running!");
        return;
    }

    //CCLOG("TimeSystem update with dt: %.3f", dt);
    
    // 确保dt和timeScale都是正数
    if (dt <= 0.0f || timeScale <= 0.0f) {
        //CCLOG("Invalid dt or timeScale: dt=%.3f, timeScale=%.3f", dt, timeScale);
        return;
    }

    // 使用double进行精确计算
    double minutesElapsed = dt * 60.0 * static_cast<double>(timeScale);
    int deltaMinutes = static_cast<int>(std::floor(minutesElapsed));

    if (deltaMinutes <= 0) return;

    // 更新分钟
    currentTime.minute += deltaMinutes;

    // 处理时间进位
    while (currentTime.minute >= MINUTES_PER_HOUR) {
        currentTime.minute -= MINUTES_PER_HOUR;
        currentTime.hour++;

        if (currentTime.hour >= HOURS_PER_DAY) {
            currentTime.hour = 0;
            currentTime.day++;  // 先增加天数

            if (currentTime.day > DAYS_PER_SEASON) {
                currentTime.day = 1;
                Season oldSeason = static_cast<Season>(currentTime.season);
                currentTime.season = (currentTime.season + 1) % 4;

                // 调用季节变化回调
                if (seasonChangedCallback) {
                    seasonChangedCallback(SEASON_NAMES[currentTime.season]);
                }

                CCLOG("Season changed from %s to %s", 
                    SEASON_NAMES[static_cast<int>(oldSeason)].c_str(),
                    SEASON_NAMES[currentTime.season].c_str());

                if (currentTime.season == 0) {
                    currentTime.year++;
                }
            }
        }
    }

    /*CCLOG("Time updated - Year: %d, Season: %d, Day: %d, %02d:%02d",
        currentTime.year, currentTime.season, currentTime.day,
        currentTime.hour, currentTime.minute);
        */
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
        seasonToString(previousSeason).c_str(),  // 使用安全的方法
        getCurrentSeasonString().c_str());       // 使用安全的方法

    */
    if (currentTime.season == 0) {  // �µ�һ��
        currentTime.year++;
        //CCLOG("Year advanced to: %d", currentTime.year);
    }

    notifySeasonChange(previousSeason);
    saveToUserDefault();
}

// �¼�֪ͨ
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

// �¼�����ע��
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

// ʱ������
void TimeSeasonSystem::setTimeScale(float scale) {
    timeScale = std::max(0.0f, scale);
    //CCLOG("Time scale set to: %.2f", timeScale);
}

void TimeSeasonSystem::setTime(const GameTime& time) {
    GameTime previousTime = currentTime;
    
    // 确保所有值都在有效范围内
    currentTime.minute = std::clamp(time.minute, 0, MINUTES_PER_HOUR - 1);
    currentTime.hour = std::clamp(time.hour, 0, HOURS_PER_DAY - 1);
    currentTime.day = std::clamp(time.day, 1, DAYS_PER_SEASON);  // 从1开始
    currentTime.season = std::clamp(time.season, 0, 3);
    currentTime.year = std::max(1, time.year);  // 至少从第1年开始

    /*CCLOG("Time set to - Year: %d, Season: %d, Day: %d, %02d:%02d",
        currentTime.year, currentTime.season, currentTime.day,
        currentTime.hour, currentTime.minute);
*/

    checkAndNotifyChanges(previousTime);
    saveToUserDefault();
}

// �浵���
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
    
    // 设置默认值，确保都是有效值
    currentTime.year = std::max(1, ud->getIntegerForKey("game_year", 2024));
    currentTime.season = std::clamp(ud->getIntegerForKey("game_season", 0), 0, 3);
    currentTime.day = std::clamp(ud->getIntegerForKey("game_day", 1), 1, DAYS_PER_SEASON);
    currentTime.hour = std::clamp(ud->getIntegerForKey("game_hour", 0), 0, HOURS_PER_DAY - 1);
    currentTime.minute = std::clamp(ud->getIntegerForKey("game_minute", 0), 0, MINUTES_PER_HOUR - 1);
    timeScale = std::max(0.1f, ud->getFloatForKey("game_time_scale", 67.2f));

    /*CCLOG("Loaded time - Year: %d, Season: %d, Day: %d, %02d:%02d",
        currentTime.year, currentTime.season, currentTime.day,
        currentTime.hour, currentTime.minute);
        */
}

// ��������
void TimeSeasonSystem::checkAndNotifyChanges(const GameTime& previousTime) {
    if (currentTime.season != previousTime.season) {
        notifySeasonChange(static_cast<Season>(previousTime.season));
    }
    else if (currentTime.day != previousTime.day) {
        notifyDayChange(previousTime.day);
    }
}

std::string TimeSeasonSystem::seasonToString(Season season) {
    size_t index = static_cast<size_t>(season);  // 使用 size_t 避免符号比较警告
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