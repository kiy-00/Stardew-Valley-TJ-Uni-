#include "Carrot.h"

Carrot* Carrot::create() {
    Carrot* carrot = new (std::nothrow) Carrot();
    if (carrot && carrot->init()) {
        carrot->autorelease();
        return carrot;
    }
    CC_SAFE_DELETE(carrot);
    return nullptr;
}

bool Carrot::init() {
    if (!Crop::init()) {
        return false;
    }

    cropType = "carrot";
    totalGrowthTime = 96.0f; // 4天

    // 设置胡萝卜的生长条件
    growthConditions.minTemp = 10.0f;
    growthConditions.maxTemp = 25.0f;
    growthConditions.preferredSeason = "Spring";
    growthConditions.waterNeed = 0.8f;

    return true;
}

void Crop::updateGrowth(float dt) {
    Crop::updateGrowth(dt);
    // 胡萝卜特有的生长逻辑
}

bool Carrot::checkGrowthConditions() {
    return isWatered; // 胡萝卜只需要水
}