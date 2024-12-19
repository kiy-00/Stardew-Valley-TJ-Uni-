#include "Crop.h"
// 具体的生长状态类


void SeedState::advance(Crop* crop) {
	crop->setGrowthState(new SproutState());
	crop->updateAppearance();
}



void SproutState::advance(Crop* crop) {
	crop->setGrowthState(new GrowingState());
	crop->updateAppearance();
}



void GrowingState::advance(Crop* crop) {
	crop->setGrowthState(new MatureState());
	crop->finalizeQuality();
	crop->updateAppearance();
}



void MatureState::advance(Crop* crop) {
	// 已经成熟，不再进行状态转换
}
