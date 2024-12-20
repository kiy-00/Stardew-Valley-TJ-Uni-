#include "LandStates.h"
#include "Farmland.h"
#include "WeatherEffectFactory.h"
#include "WeatherEffects.h"

void NormalState::till(Farmland* land) {
	// 普通状态可以开垦
	land->setState(new TilledState());
}

void NormalState::water(Farmland* land) {
	// 普通状态不能直接浇水
	CCLOG("Cannot water untilled land!");
}

void NormalState::fertilize(Farmland* land) {
	//普通状态不能施肥
	CCLOG("Cannot fertilize untilled land!");
}

void NormalState::plant(Farmland* land, CropType cropType) {
	// 普通状态不能种植
	CCLOG("Cannot plant in untilled land!");
}
void NormalState::update(Farmland* land, float dt) {
	// 普通土地状态无需更新
	CCLOG("Normal state update");
}
void NormalState::weatherEffect(Farmland* land, WeatherType weatherType) {
	auto effect = WeatherEffectFactory::createEffect(weatherType);
	if (effect) {
		effect->applyToNormal(land);
		effect->applyToCrop(land->getCrop());
	}
}

void NormalState::updateCropGrowth(Farmland* land) {
	;
}