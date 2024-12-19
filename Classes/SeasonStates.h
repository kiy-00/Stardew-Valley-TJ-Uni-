#pragma once

#ifndef __SEASON_STATES_H__
#define __SEASON_STATES_H__
#include"SeasonSystem.h"

class SeasonSystem;
class SpringState :public ISeasonState {
public:
	virtual SeasonType getSeasonName() const override {
		return SeasonType::SPRING;
	}
	virtual void changeToNextSeason(SeasonSystem* seasonSystem)const override;
};

class SummerState :public ISeasonState {
public:
	virtual SeasonType getSeasonName() const override {
		return SeasonType::SUMMER;
	}
	virtual void changeToNextSeason(SeasonSystem* seasonSystem)const override;
};

class AutumnState :public ISeasonState {
public:
	virtual SeasonType getSeasonName() const override {
		return SeasonType::AUTUMN;
	}
	virtual void changeToNextSeason(SeasonSystem* seasonSystem)const override;
};

class WinterState :public ISeasonState {
public:
	virtual SeasonType getSeasonName() const override {
		return SeasonType::WINTER;
	}
	virtual void changeToNextSeason(SeasonSystem* seasonSystem)const override;
};

#endif