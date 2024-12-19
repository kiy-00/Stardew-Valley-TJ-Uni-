// 实现文件
#include "SeasonSystem.h"
#include "ISeasonObserver.h"
#include "SeasonStates.h"

SeasonSystem* SeasonSystem::instance = nullptr;

SeasonSystem* SeasonSystem::getInstance() {
    if (instance == nullptr) {
        instance = new SeasonSystem();
        instance->init();
    }
    return instance;
}

void SeasonSystem::destroyInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

SeasonSystem::SeasonSystem()
    : currentSeason(new SummerState())
    , currentDay(1)
    , currentYear(1)
    , accumulatedTime(0.0f)
{
}

SeasonSystem::~SeasonSystem() {
    
}

void SeasonSystem::update(float dt) {
    accumulatedTime += dt;

    // 假设1游戏分钟 = 1现实秒
    // 1天 = 24游戏小时 = 1440游戏分钟
    const float TIME_PER_DAY = 144.0f;

    if (accumulatedTime >= TIME_PER_DAY) {
        // 增加天数
        currentDay++;
        accumulatedTime -= TIME_PER_DAY;

        // 每季节28天
        if (currentDay > 28) {
            currentDay = 1;
            changeSeason();
        }
    }
}

void SeasonSystem::setCurrentSeason(ISeasonState* newSeason) {
    if (currentSeason)
        delete currentSeason;
    currentSeason = newSeason;
}

void SeasonSystem::changeSeason() {
    // 切换到下一个季节
    currentSeason->changeToNextSeason(this);
    notifyObservers();

    // 如果完成了一年的循环
    if (currentSeason->getSeasonName()==SeasonType::SPRING) {
        currentYear++;
    }
 
}

void SeasonSystem::addObserver(ISeasonObserver* observer) {
    m_observers.push_back(observer);
}

void SeasonSystem::removeObserver(ISeasonObserver* observer) {
    m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
}

void SeasonSystem::notifyObservers() {
    // 通知所有观察者
    for (auto observer : m_observers) {
        observer->onSeasonChanged(this);
    }
}