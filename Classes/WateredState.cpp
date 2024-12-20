#include "LandStates.h"
#include "Farmland.h"
#include "WeatherEffectFactory.h"
#include "CropFactory.h"
#include "WeatherEffects.h"

void WateredState::till(Farmland* land) {
	// 已浇水的土地不能开垦
	CCLOG("Cannot till watered land!");
}

void WateredState::water(Farmland* land) {
	// 重置湿度持续时间
	land->setWaterTimer(0.0f);
	CCLOG("Refreshed water!");
}

void WateredState::fertilize(Farmland* land) {
	land->setState(new WateredAndFertilizedState());
}

void WateredState::plant(Farmland* land, CropType cropType) {
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

	// 更新土地的作物指针
	land->setCrop(newCrop);

	CCLOG("Successfully planted a %s!", newCrop->getName().c_str());
}

void WateredState::update(Farmland* land, float dt) {
	updateCropGrowth(land);
	updateWaterTimer(land, dt);
	// 水分会随时间蒸发
	float timer = land->getWaterTimer();
	if (timer >= wateringDuration) {  // 24小时后水分蒸发
		transitionToTilledState(land);//转回开垦状态
		CCLOG("Land dried up, returning to tilled state");
	}
}

void WateredState::weatherEffect(Farmland* land, WeatherType weatherType) {
	auto effect = WeatherEffectFactory::createEffect(weatherType);
	if (effect) {
		effect->applyToWatered(land);
		effect->applyToCrop(land->getCrop());
	}
}
void WateredState::updateCropGrowth(Farmland* land) {
	// 已开垦且浇水，正常生长
	Crop* crop = land->getCrop();
	if (crop) {
		crop->updateGrowthFromLand(true, false, 50.0f);  // 基础肥力50%
		land->setCropGrowthRate(1.0f);  // 设置作物生长速度为正常
	}
}