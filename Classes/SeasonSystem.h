#ifndef __SEASON_SYSTEM_H__
#define __SEASON_SYSTEM_H__

#include "cocos2d.h"
#include "ISeasonState.h"

class ISeasonObserver;
class SeasonSystem : public cocos2d::Ref {
public:
    static SeasonSystem* getInstance();
    static void destroyInstance();

    // 更新季节（传入游戏时间）
    void update(float dt);

    
    SeasonType getCurrentSeason() const { return currentSeason->getSeasonName(); }
    void setCurrentSeason(ISeasonState* newSeason);
    ISeasonState* getSeasonState() { return currentSeason; }


    // 获取当前季节的天数(1-28)
    int getCurrentDay() const { return currentDay; }

    // 获取当前年份
    int getCurrentYear() const { return currentYear; }


    //观察者相关
    void addObserver(ISeasonObserver* observer);
    void removeObserver(ISeasonObserver* observer);
    void notifyObservers();

    // 季节转换
    void changeSeason();

private:
    SeasonSystem();
    ~SeasonSystem();

    static SeasonSystem* instance;//季节系统单例

    int currentDay;
    int currentYear;
    float accumulatedTime;  // 累积的游戏时间

    std::vector<ISeasonObserver*>m_observers;//观察者列表

    ISeasonState* currentSeason;//记录当前季节

};

#endif