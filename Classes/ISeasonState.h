#pragma once
#ifndef __I_SEASON_STATE_H__
#define __I_SEASON_STATE_H__

#include "Crop.h"
class SeasonSystem;
class ISeasonState {
public:
	virtual void changeToNextSeason(SeasonSystem* seasonSystem)const=0;
	virtual void canCropGrow(Crop* crop) {
		std::vector<SeasonType> seasonList = crop->getSupportSeason();
		for (SeasonType season : seasonList)
			if (season == getSeasonName())
				return;
		crop->died();
	};
	virtual SeasonType getSeasonName() const = 0;
	virtual ~ISeasonState() {};

};
#endif