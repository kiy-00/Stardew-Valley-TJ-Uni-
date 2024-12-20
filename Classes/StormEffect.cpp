#include "WeatherEffects.h"

void StormEffect::applyToNormal(Farmland* land) {
	land->setState(new TilledState());
	CCLOG("Storm effect: Soil erosion, land state changed to Tilled.");
}
void StormEffect::applyToTilled(Farmland* land) {
	land->setWaterTimer(0.0f);  // 清除水分计时器
	land->setState(new TilledState());  // 转为开垦状态
	CCLOG("Storm effect: Water evaporated, land state changed to Tilled.");
}
void StormEffect::applyToWatered(Farmland* land) {
	land->setWaterTimer(0.0f);  // 清除水分计时器
	land->setState(new TilledState());  // 转为开垦状态
	CCLOG("Storm effect: Water evaporated and land is affected.");
}
void StormEffect::applyToFertilized(Farmland* land) {
	land->setFertilizeTimer(0.0f);  // 清除肥料计时器
	land->setState(new TilledState());  // 转为开垦状态
	CCLOG("Storm effect: Fertilizer washed away, land state changed to Tilled.");
}
void StormEffect::applyToWateredAndFertilized(Farmland* land) {
	land->setWaterTimer(0.0f);  // 清除水分计时器
	land->setFertilizeTimer(0.0f);  // 清除肥料计时器
	land->setState(new FertilizedState());  // 转为施肥状态
	CCLOG("Storm effect: Water evaporated and fertilizer washed away, land state changed to Fertilized.");
}
void StormEffect::applyToCrop(Crop* crop) {
	if (crop != nullptr) {
		crop->setGrowthRate(0.5f);  // 风暴，作物生长减慢
	}
}