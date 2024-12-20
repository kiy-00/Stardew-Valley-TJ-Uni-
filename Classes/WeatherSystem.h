#pragma once
#ifndef __WEATHER_SYSTEM_H__
#define __WEATHER_SYSTEM_H__

#include "cocos2d.h"
#include "Enums.h"
#include <vector>
#include "WeatherStates.h"

// 前向声明
class Farmland;
class IWeatherState;
class IWeatherObserver;

class WeatherSystem : public cocos2d::Node {
public:
	// 单例模式
	static WeatherSystem* getInstance();

	// 基本天气功能
	WeatherType getCurrentWeather() const { return m_currentWeather->getWeatherName(); }
	
	// 累积天气持续时间
	void update(float dt);

	//观察者相关
	void addObserver(IWeatherObserver* observer);
	void removeObserver(IWeatherObserver* observer);
	void notifyObservers();

	//天气转换
	void changeWeather(IWeatherState* newState);

private:
	WeatherSystem();
	~WeatherSystem();

	static WeatherSystem* s_instance;//天气系统单例


	float m_weatherTimer;		//记录当前天气持续了多长时间
	float m_weatherDuration;    //指定多久尝试改变一次天气

	std::vector<IWeatherObserver*> m_observers;  // 观察者列表

	IWeatherState* m_currentWeather;//记录当前天气
};

#endif // __WEATHER_SYSTEM_H__