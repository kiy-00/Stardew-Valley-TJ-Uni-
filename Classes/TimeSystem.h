#pragma once
#ifndef __TIME_SYSTEM_H__
#define __TIME_SYSTEM_H__

#include "cocos2d.h"
#include "Enums.h"
#include <vector>

class TimeSystem {
public:
	// 单例模式
	static TimeSystem* getInstance() {
		static TimeSystem instance;
		return &instance;
	}

	// 时间获取
	int getCurrentHour() const { return m_hour; }
	int getCurrentDay() const { return m_day; }
	Season getCurrentSeason() const { return m_currentSeason; }
	int getYear() const { return m_year; }

	// 时间更新
	void update(float dt);

	// 时间转换
	float getGameTimeScale() const { return m_gameTimeScale; }
	void setGameTimeScale(float scale) { m_gameTimeScale = scale; }

	// 事件注册
	using TimeCallback = std::function<void()>;
	void registerDayChangeCallback(const TimeCallback& callback);
	void registerSeasonChangeCallback(const TimeCallback& callback);

private:
	TimeSystem()
		: m_hour(6)  // 游戏从早上6点开始
		, m_day(1)
		, m_currentSeason(Season::SPRING)
		, m_year(1)
		, m_gameTimeScale(60.0f)  // 1现实秒 = 1游戏分钟
		, m_timeAccumulator(0.0f)
	{}

	int m_hour;
	int m_day;
	Season m_currentSeason;
	int m_year;
	float m_gameTimeScale;
	float m_timeAccumulator;

	std::vector<TimeCallback> m_dayChangeCallbacks;
	std::vector<TimeCallback> m_seasonChangeCallbacks;

	void advanceHour();
	void advanceDay();
	void advanceSeason();
	void notifyDayChange();
	void notifySeasonChange();

	static const int HOURS_PER_DAY = 24;
	static const int DAYS_PER_SEASON = 28;
	static const int SEASONS_PER_YEAR = 4;
};

#endif // __TIME_SYSTEM_H__