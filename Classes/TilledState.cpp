#include "LandStates.h"
#include "Farmland.h"
#include "WeatherEffectFactory.h"
#include "WeatherEffects.h"

void TilledState::till(Farmland* land) {
	// 已开垦的土地再次开垦无效
	CCLOG("Land is already tilled!");
}

void TilledState::water(Farmland* land) {
	// 开垦的土地可以浇水
	land->setState(new WateredState());
}

void TilledState::fertilize(Farmland* land) {
	//开垦过的土地可以施肥
	land->setState(new FertilizedState());
}

void TilledState::plant(Farmland* land, CropType cropType) {
	// 开垦的土地可以种植，但建议先浇水
	CCLOG("Land is dry! Consider watering first.");
	// 这里可以选择是否允许在干燥土地种植
}

void TilledState::update(Farmland* land, float dt) {
	updateCropGrowth(land);
	CCLOG("Tilled state update");
}

void TilledState::weatherEffect(Farmland* land, WeatherType weatherType) {
	auto effect = WeatherEffectFactory::createEffect(weatherType);
	if (effect) {
		effect->applyToTilled(land);
		effect->applyToCrop(land->getCrop());
	}
}
void TilledState::updateCropGrowth(Farmland* land) {
	// 已开垦但未浇水，生长较慢
	Crop* crop = land->getCrop();
	if (crop) {
		crop->updateGrowthFromLand(false, false, 30.0f);  // 基础肥力30%
		land->setCropGrowthRate(0.8f);  // 设置作物生长速度为较慢
	}
}