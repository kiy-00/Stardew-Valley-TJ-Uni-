#pragma once
// LandStates.h
#ifndef __LAND_STATES_H__
#define __LAND_STATES_H__

#include "ILandState.h"

class NormalState : public ILandState {
public:

	void till(Farmland* land) override;
	void water(Farmland* land) override;
	void fertilize(Farmland* land) override;
	void plant(Farmland* land, CropType cropType) override;
	void update(Farmland* land, float dt) override;

	void weatherEffect(Farmland* land, WeatherType weatherType)override;

	void updateCropGrowth(Farmland* land)override;

	LandState getStateName() const override { return LandState::NORMAL; }
	std::string getTexturePath() const override {
		return "farmland/normal.png";
	};

	bool isWatered() const override { return false; }
	bool isTilled() const override { return false; }
	bool isFertilized() const override { return false; }
	bool hasCrop() const override { return false; }
};

class TilledState : public ILandState {
public:
	void till(Farmland* land) override;
	void water(Farmland* land) override;
	void fertilize(Farmland* land) override;
	void plant(Farmland* land, CropType cropType) override;
	void update(Farmland* land, float dt) override;

	void weatherEffect(Farmland* land, WeatherType weatherType)override;

	void updateCropGrowth(Farmland* land)override;

	LandState getStateName() const override { return LandState::TILLED; }
	std::string getTexturePath() const override {
		return "farmland/tilled.png";
	};

	bool isWatered() const override { return false; }
	bool isTilled() const override { return true; }
	bool isFertilized() const override { return false; }
	bool hasCrop() const override { return false; }
};

class WateredState : public ILandState {
public:
	void till(Farmland* land) override;
	void water(Farmland* land) override;
	void fertilize(Farmland* land) override;
	void plant(Farmland* land, CropType cropType) override;
	void update(Farmland* land, float dt) override;

	void weatherEffect(Farmland* land, WeatherType weatherType)override;

	void updateCropGrowth(Farmland* land)override;

	LandState getStateName() const override { return LandState::WATERED; }
	std::string getTexturePath() const override {
		return "farmland/watered.png";
	};

	bool isWatered() const override { return true; }
	bool isTilled() const override { return true; }
	bool isFertilized() const override { return false; }
	bool hasCrop() const override { return false; }

};

class FertilizedState : public ILandState {
public:
	void till(Farmland* land) override;
	void water(Farmland* land) override;
	void fertilize(Farmland* land) override;
	void plant(Farmland* land, CropType cropType) override;
	void update(Farmland* land, float dt) override;

	void weatherEffect(Farmland* land, WeatherType weatherType)override;

	void updateCropGrowth(Farmland* land)override;

	LandState getStateName() const override { return LandState::FERTILIZED; }
	std::string getTexturePath() const override {;
		return "farmland/fertilized.png";
	};

	bool isWatered() const override { return false; }
	bool isTilled() const override { return true; }
	bool isFertilized() const override { return true; }
	bool hasCrop() const override { return false; }
};

class WateredAndFertilizedState : public ILandState {
public:
	void till(Farmland* land) override;
	void water(Farmland* land) override;
	void fertilize(Farmland* land) override;
	void plant(Farmland* land, CropType cropType) override;
	void update(Farmland* land, float dt) override;

	void weatherEffect(Farmland* land, WeatherType weatherType)override;

	void updateCropGrowth(Farmland* land)override;

	LandState getStateName() const override { return LandState::WATERED_AND_FERTILIZED; }
	std::string getTexturePath() const override {
		return "farmland/watered_fertilized.png";
	};

	bool isWatered() const override { return true; }
	bool isTilled() const override { return true; }
	bool isFertilized() const override { return true; }
	bool hasCrop() const override { return false; }
};
#endif // __LAND_STATES_H__