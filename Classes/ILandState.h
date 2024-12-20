#pragma once
#ifndef __I_LAND_STATE_H__
#define __I_LAND_STATE_H__

#include "cocos2d.h"
#include "Enums.h"
#include "SeasonSystem.h"

class Farmland; // 前向声明

class ILandState {
protected:
	float wateringDuration;
	float fertilizingDuration;
public:
	ILandState() :wateringDuration(240.0f), fertilizingDuration(480.0f){};
	virtual ~ILandState() = default;
	virtual void till(Farmland* land) = 0;
	virtual void water(Farmland* land) = 0;
	virtual void fertilize(Farmland* land) = 0;
	virtual void plant(Farmland* land, CropType cropType) = 0;
	virtual void update(Farmland* land, float dt) = 0;
	virtual void weatherEffect(Farmland* land, WeatherType weatherType) = 0;

	//更新函数
	virtual void updateCropGrowth(Farmland* land) = 0;
	void updateWaterTimer(Farmland* land, float dt);
	void updateFertilizeTimer(Farmland* land, float dt);
	// 状态转换函数
	void transitionToTilledState(Farmland* land);
	void transitionToFertilizedState(Farmland* land);
	void transitionToWateredState(Farmland* land);

	virtual LandState getStateName() const = 0;
	virtual std::string getTexturePath() const = 0;
	virtual bool isWatered() const = 0;
	virtual bool isTilled() const = 0;
	virtual bool isFertilized() const = 0;
	virtual bool hasCrop() const = 0;
	virtual bool canGrowInSeason(Crop* newCrop)const {
		std::vector<SeasonType> seasonList = newCrop->getSupportSeason();
		SeasonType currentSeason = SeasonSystem::getInstance()->getCurrentSeason();
		for (SeasonType season : seasonList)
			if (season == currentSeason)
				return true;
		return false;
	}
};



#endif // __I_LAND_STATE_H__