#include "WeatherEffects.h"

void SnowyEffect::applyToNormal(Farmland* land) {
	CCLOG("Snowy effect: No effect on unplowed land.");
}
void SnowyEffect::applyToTilled(Farmland* land) {
	land->setWaterTimer(0.0f);  // 清除水分计时器
	land->setState(new TilledState());  // 转为开垦状态
	CCLOG("Snowy effect: Water froze, land state changed to Tilled.");
}
void SnowyEffect::applyToWatered(Farmland* land) {
	land->setWaterTimer(0.0f);  // 清除水分计时器
	land->setState(new TilledState());  // 转为开垦状态
	CCLOG("Snowy effect: Water froze, land state changed to Tilled.");
}
void SnowyEffect::applyToFertilized(Farmland* land) {
	land->setFertilizeTimer(0.0f);  // 清除肥料计时器
	land->setState(new TilledState());  // 转为开垦状态
	CCLOG("Snowy effect: Fertilizer invalidated, land state changed to Tilled.");
}
void SnowyEffect::applyToWateredAndFertilized(Farmland* land) {
	land->setWaterTimer(0.0f);  // 清除水分计时器
	land->setFertilizeTimer(0.0f);  // 清除肥料计时器
	land->setState(new TilledState());  // 转为开垦状态
	CCLOG("Snowy effect: Water froze and fertilizer lost, land state changed to Tilled.");
}
void SnowyEffect::applyToCrop(Crop* crop) {
	if (crop != nullptr) {
		crop->setGrowthRate(0);  // 雪天，作物生长停止
	}
}