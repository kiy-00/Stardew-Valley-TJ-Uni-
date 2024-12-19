#pragma once
#ifndef __WEATHER_EFFECTS_H__
#define __WEATHER_EFFECTS_H__
#include "IWeatherEffect.h"

class SunnyEffect : public WeatherEffect {

public:
	void applyToNormal(Farmland* land) override;
	void applyToTilled(Farmland* land) override;
	void applyToWatered(Farmland* land)override;
	void applyToFertilized(Farmland* land)override;
	void applyToWateredAndFertilized(Farmland* land)override;
	void applyToCrop(Crop* crop)override;

};

class RainyEffect : public WeatherEffect {

public:
	void applyToNormal(Farmland* land) override;
	void applyToTilled(Farmland* land) override;
	void applyToWatered(Farmland* land)override;
	void applyToFertilized(Farmland* land)override;
	void applyToWateredAndFertilized(Farmland* land)override;
	void applyToCrop(Crop* crop)override;

};

class StormEffect : public WeatherEffect {

public:
	void applyToNormal(Farmland* land) override;
	void applyToTilled(Farmland* land) override;
	void applyToWatered(Farmland* land)override;
	void applyToFertilized(Farmland* land)override;
	void applyToWateredAndFertilized(Farmland* land)override;
	void applyToCrop(Crop* crop)override;

};

class SnowyEffect : public WeatherEffect {

public:
	void applyToNormal(Farmland* land) override;
	void applyToTilled(Farmland* land) override;
	void applyToWatered(Farmland* land)override;
	void applyToFertilized(Farmland* land)override;
	void applyToWateredAndFertilized(Farmland* land)override;
	void applyToCrop(Crop* crop)override;

};
#endif