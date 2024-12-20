#include "LandStates.h"
#include "Farmland.h"
#include "WeatherEffectFactory.h"
#include "WeatherEffects.h"

void FertilizedState::till(Farmland* land) {
	CCLOG("Cannot till fertilized land!");
}

void FertilizedState::water(Farmland* land) {
	land->setState(new WateredAndFertilizedState());
}

void FertilizedState::fertilize(Farmland* land) {
	CCLOG("Land is already fertilized!");
}

void FertilizedState::plant(Farmland* land, CropType cropType) {
	CCLOG("Better water the land first!");
}

void FertilizedState::update(Farmland* land, float dt) {
	updateCropGrowth(land);

	updateFertilizeTimer(land, dt);
	float timer = land->getFertilizeTimer();
	if (timer >= fertilizingDuration) {  // 48小时后肥力消失
		transitionToTilledState(land);//转回开垦状态
		CCLOG("Fertilizer depleted");
	}
	CCLOG("Fertilized state update");
}

void FertilizedState::weatherEffect(Farmland* land, WeatherType weatherType) {
	auto effect = WeatherEffectFactory::createEffect(weatherType);
	if (effect) {
		effect->applyToFertilized(land);
		effect->applyToCrop(land->getCrop());
	}
}

void FertilizedState::updateCropGrowth(Farmland* land) {
	// 已开垦且施肥，但未浇水
	Crop* crop = land->getCrop();
	if (crop) {
		crop->updateGrowthFromLand(false, true, 70.0f);  // 基础肥力70%
		land->setCropGrowthRate(1.2f);  // 设置作物生长速度为加速
	}
}