#pragma once
#ifndef __FARMLAND_H__
#define __FARMLAND_H__

#include "ILandState.h"
#include "Position.h"
#include "WeatherSystem.h"
#include "SeasonSystem.h"
#include "ITool.h"  // 添加工具接口头文件
#include "IWeatherObserver.h"
#include "ISeasonObserver.h"

class Crop;
class Farmland : public cocos2d::Sprite,public IWeatherObserver,public ISeasonObserver {
public:

	static Farmland* create(const Position& pos);
	virtual bool init(const Position& pos);

	// 使用工具
	bool handleTool(ITool* tool);

	// 基本操作
	void till();					//开垦
	void water();					//浇水
	void fertilize();				//施肥
	void plant(CropType cropType);  //耕种
	bool harvest();					//收获
	void update(float dt) override;

	//获得当前土地上耕种的作物
	Crop* getCrop() {
		return m_currentCrop;
	}

	// 状态相关
	void setState(ILandState* newState);
	ILandState* getState() const { return m_currentState; }
	bool isWatered() const { return m_currentState->isWatered(); }
	bool isTilled() const { return m_currentState->isTilled(); }
	bool isFertilized() const { return m_currentState->isFertilized(); }
	bool hasCrop() const { return m_currentCrop != nullptr; }

	// 位置相关
	Position getGridPosition() const { return m_gridPosition; }
	void setGridPosition(const Position& pos) { m_gridPosition = pos; }

	// 天气相关
	virtual ~Farmland() {
		WeatherSystem::getInstance()->removeObserver(this);
		SeasonSystem::getInstance()->removeObserver(this);
	}

	void onWeatherChanged(const WeatherType weatherType)override;
	void onSeasonChanged(SeasonSystem* seasonSystem)override;

	void setCrop(Crop* newCrop);
	void removeCrop();

	float getWaterTimer() { return m_waterTimer; }
	void setWaterTimer(float timer) { m_waterTimer = timer; }
	float getFertilizeTimer() { return m_fertilizeTimer; }
	void setFertilizeTimer(float timer) { m_fertilizeTimer = timer; }
	void setCropGrowthRate(float growthRate);

private:
	void updateAppearance();

	ILandState* m_currentState;
	Position m_gridPosition;
	Crop* m_currentCrop;

	// 土地状态
	float m_waterTimer;
	float m_fertilizeTimer;
	float m_fertility;


};
#endif // __FARMLAND_H__