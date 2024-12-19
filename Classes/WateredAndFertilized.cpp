#include "LandStates.h"
#include "Farmland.h"
#include "WeatherEffectFactory.h"
#include "CropFactory.h"
#include "WeatherEffects.h"

void WateredAndFertilizedState::till(Farmland* land) {
	CCLOG("Cannot till watered and fertilized land!");
}

void WateredAndFertilizedState::water(Farmland* land) {
	CCLOG("Land is already watered!");
}

void WateredAndFertilizedState::fertilize(Farmland* land) {
	CCLOG("Land is already fertilized!");
}

void WateredAndFertilizedState::plant(Farmland* land, CropType cropType) {
	// 检查土地是否已有作物
	if (land->hasCrop()) {
		CCLOG("Cannot plant: There is already a crop in this land!");
		return;
	}

	// 使用工厂创建指定类型的作物
	Crop* newCrop = CropFactory::getInstance()->createCrop(cropType);
	if (!newCrop) {
		CCLOG("Failed to create crop!");
		return;
	}
	if (!canGrowInSeason(newCrop)) {
		CCLOG("Can't grow the crop in current season.");
		return;
	}


	// 获取土地的尺寸和位置信息
	cocos2d::Size landSize = land->getContentSize();

	// 将作物添加为土地的子节点
	land->addChild(dynamic_cast<cocos2d::Node*>(newCrop));
	newCrop->setPosition(cocos2d::Vec2(landSize.width / 2, landSize.height / 2));

	// 因为是最理想状态，可以给作物一些初始成长加成
	newCrop->updateGrowthFromLand(true, true, 100.0f); // 立即给予最佳生长条件的加成

	// 更新土地的作物指针
	land->setCrop(newCrop);

	CCLOG("Successfully planted a %s in ideal conditions!", newCrop->getName().c_str());
}

void WateredAndFertilizedState::update(Farmland* land, float dt) {
	updateCropGrowth(land);
	updateWaterTimer(land, dt);
	updateFertilizeTimer(land, dt);

	float waterTimer = land->getWaterTimer();
	float fertilizeTimer = land->getFertilizeTimer();
	bool waterDepleted = waterTimer >= wateringDuration;
	bool fertilizerDepleted = fertilizeTimer >= fertilizingDuration;

	if (waterDepleted && fertilizerDepleted) {
		transitionToTilledState(land);
	}
	else if (waterDepleted) {
		transitionToFertilizedState(land);
	}
	else if (fertilizerDepleted) {
		transitionToWateredState(land);
	}

	CCLOG("WateredAndFertilized state update");
}

void WateredAndFertilizedState::weatherEffect(Farmland* land, WeatherType weatherType) {
	auto effect = WeatherEffectFactory::createEffect(weatherType);
	if (effect) {
		effect->applyToWateredAndFertilized(land);
		effect->applyToCrop(land->getCrop());
	}
}
void WateredAndFertilizedState::updateCropGrowth(Farmland* land) {
	// 最理想状态，生长加速
	Crop* crop = land->getCrop();
	if (crop) {
		crop->updateGrowthFromLand(true, true, 100.0f);  // 最高肥力100%
		land->setCropGrowthRate(1.5f);  // 设置作物生长速度为加速
	}
}