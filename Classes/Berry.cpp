#include "Berry.h"

Berry* Berry::create() {
    Berry* berry = new (std::nothrow) Berry();
    if (berry && berry->init()) {
        berry->autorelease();
        return berry;
    }
    CC_SAFE_DELETE(berry);
    return nullptr;
}

bool Berry::init() {
    if (!Crop::init()) {
        return false;
    }

    cropType = "berry";
    totalGrowthTime = 72.0f; // 3天

    // 设置浆果的生长条件
    growthConditions.minTemp = 15.0f;
    growthConditions.maxTemp = 30.0f;
    growthConditions.preferredSeason = "Summer";
    growthConditions.waterNeed = 1.0f;

    return true;
}

void Berry::updateGrowth(float dt) {
    Crop::updateGrowth(dt);
    // 浆果特有的生长逻辑
}

bool Berry::checkGrowthConditions() {
    return isWatered && // 需要水
        isFertilized; // 需要肥料
}
