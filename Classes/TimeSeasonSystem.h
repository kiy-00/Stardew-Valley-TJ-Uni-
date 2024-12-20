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
    // ����ö��
    enum class Season {
        SPRING,
        SUMMER,
        FALL,
        WINTER
    };

    // ʱ����صĽṹ��
    struct GameTime {
        int year;       // ��Ϸ���
        int season;     // ����(0-3)
        int day;        // ��ǰ����(1-28)
        int hour;       // Сʱ(0-23)
        int minute;     // ����(0-59)

        bool operator==(const GameTime& other) const {
            return year == other.year && season == other.season &&
                day == other.day && hour == other.hour && minute == other.minute;
        }
    };

    // �¼����ݽṹ
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

    // ��������
    static Season intToSeason(int seasonInt);
    static std::string seasonToString(Season season);

    // �����ͳ�ʼ��
    static TimeSeasonSystem* getInstance();
    virtual bool init() override;

    // ʱ�����
    void startTime();
    void pauseTime();
    void resumeTime();

    // ʱ��ͼ��ڻ�ȡ
    const GameTime& getCurrentTime() const { return currentTime; }
    Season getCurrentSeason() const { return static_cast<Season>(currentTime.season); }
    std::string getCurrentSeasonString() const;
    int getCurrentDay() const { return currentTime.day; }
    int getCurrentYear() const { return currentTime.year; }

    // ʱ������
    void setTimeScale(float scale); // ����ʱ������
    void setTime(const GameTime& time); // ֱ������ʱ��

    // �¼�����ע��
    using SeasonChangeCallback = std::function<void(const SeasonChangeEvent&)>;
    using DayChangeCallback = std::function<void(const DayChangeEvent&)>;

    void addSeasonChangeListener(const std::string& name, const SeasonChangeCallback& callback);
    void addDayChangeListener(const std::string& name, const DayChangeCallback& callback);
    void removeSeasonChangeListener(const std::string& name);
    void removeDayChangeListener(const std::string& name);

    // �浵���
    void saveToUserDefault();
    void loadFromUserDefault();

    // ����
    static const int DAYS_PER_SEASON = 28;
    static const int HOURS_PER_DAY = 24;
    static const int MINUTES_PER_HOUR = 60;
    static const std::vector<std::string> SEASON_NAMES;
    const float REAL_SECONDS_PER_GAME_MINUTE = 1.0f;  // 现实1秒 = 游戏1分钟
    const float MINUTES_PER_SEASON = 60.0f;           // 游戏60分钟 = 1个季节

    void setSeasonChangedCallback(const std::function<void(const std::string&)>& callback);

protected:
    TimeSeasonSystem();
    virtual ~TimeSeasonSystem();

private:
    static TimeSeasonSystem* instance;

    // ��������
    GameTime currentTime;
    float timeScale;
    float accumulatedTime;
    bool isRunning;

    // �ص��洢
    std::map<std::string, SeasonChangeCallback> seasonChangeCallbacks;
    std::map<std::string, DayChangeCallback> dayChangeCallbacks;

    std::function<void(const std::string&)> seasonChangedCallback;

    // ���·���
    void update(float dt) override;
    void updateTime(float deltaMinutes);

    // �¼�֪ͨ
    void notifySeasonChange(Season previousSeason);
    void notifyDayChange(int previousDay);

    // ʱ����㸨������
    void advanceDay();
    void advanceSeason();
    void checkAndNotifyChanges(const GameTime& previousTime);

    
};

#endif // __TIME_SEASON_SYSTEM_H__