#include "WeatherEffects.h"

void RainyEffect::applyToNormal(Farmland* land) {
	CCLOG("Rainy effect: No effect on unplowed land.");
}
void RainyEffect::applyToTilled(Farmland* land) {
	land->setState(new WateredState());
	CCLOG("Rainy effect: Water added, land state changed to Watered.");
}
void RainyEffect::applyToWatered(Farmland* land) {
	land->setWaterTimer(land->getWaterTimer() + 1.0f);  // 增加水分计时
	CCLOG("Rainy effect: Land is watered more.");
}
void RainyEffect::applyToFertilized(Farmland* land) {
	land->setWaterTimer(land->getWaterTimer() + 1.0f);  // 增加水分计时
	CCLOG("Rainy effect: Land watered, but fertilizer may be reduced.");
}
void RainyEffect::applyToWateredAndFertilized(Farmland* land) {
	land->setWaterTimer(land->getWaterTimer() + 1.0f);  // 增加水分
	CCLOG("Rainy effect: Land watered more, staying in WateredAndFertilized state.");
}
void RainyEffect::applyToCrop(Crop* crop) {
	if (crop != nullptr) {
		crop->setGrowthRate(0.5f);  // 风暴，作物生长减慢
	}
}