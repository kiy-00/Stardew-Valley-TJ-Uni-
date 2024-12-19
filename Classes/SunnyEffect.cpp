#include "WeatherEffects.h"

void SunnyEffect::applyToNormal(Farmland* land) {
	CCLOG("Sunny effect: No effect on unplowed land.");
}

void SunnyEffect::applyToTilled(Farmland* land) {
	land ->setWaterTimer(0.0f);  // 清除水分计时器
	land->setState(new TilledState());  // 转为开垦状态
	CCLOG("Sunny effect: Water evaporated, land state changed to Tilled.");
}
 
void SunnyEffect::applyToWatered(Farmland* land) {
	land->setWaterTimer(0.0f);  // 清除水分计时器
	land->setState(new TilledState());  // 转为开垦状态
	CCLOG("Sunny effect: Water evaporated, land state changed to Tilled.");
}

void SunnyEffect::applyToFertilized(Farmland* land) {
	CCLOG("Sunny effect: No effect on fertilized land.");
}

void SunnyEffect::applyToWateredAndFertilized(Farmland* land) {
	land->setWaterTimer(0.0f);  // 清除水分计时器
	land->setState(new FertilizedState());  // 转为施肥状态
	CCLOG("Sunny effect: Water evaporated, land state changed to Fertilized.");
}
void SunnyEffect::applyToCrop(Crop* crop) {
	if (crop != nullptr) {
		crop->setGrowthRate(1.0f);  // 晴天，作物正常生长
	}
}