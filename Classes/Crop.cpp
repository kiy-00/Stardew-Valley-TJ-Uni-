#include "Crop.h"
#include "Berry.h"
#include "Carrot.h"

Crop* Crop::create(const std::string& cropType) {
    if (cropType == "Berry") return Berry::create();
    if (cropType == "Carrot") return Carrot::create();
    // Add other crop types here
    return nullptr;
}

bool Crop::init() {
    if (!Node::init()) {
        return false;
    }

    currentStage = CropStage::SEED;
    growthTime = 0;
    totalGrowthTime = 100.0f; // Base growth time
    isWatered = false;
    isFertilized = false;

    // 初始化精灵
    cropSprite = Sprite::create();
    this->addChild(cropSprite);

    // 设置更新回调
    this->schedule([this](float dt) {
        this->updateGrowth(dt);
        }, 1.0f, "crop_growth");

    return true;
}

void Crop::updateGrowth(float dt) {
    if (currentStage == CropStage::DEAD || currentStage == CropStage::MATURE) {
        return;
    }

    if (!checkGrowthConditions()) {
        currentStage = CropStage::DEAD;
        updateSprite();
        return;
    }

    float growthRate = 1.0f;
    if (isWatered) growthRate *= 1.5f;
    if (isFertilized) growthRate *= 2.0f;

    growthTime += dt * growthRate;

    // 更新生长阶段
    if (growthTime >= totalGrowthTime) {
        currentStage = CropStage::MATURE;
    }
    else if (growthTime >= totalGrowthTime * 0.8f) {
        currentStage = CropStage::GROWING_3;
    }
    else if (growthTime >= totalGrowthTime * 0.6f) {
        currentStage = CropStage::GROWING_2;
    }
    else if (growthTime >= totalGrowthTime * 0.3f) {
        currentStage = CropStage::GROWING_1;
    }

    updateSprite();
}

void Crop::updateSprite() {
    std::string imagePath = cropType + "_";
    switch (currentStage) {
        case CropStage::SEED:
            imagePath += "1.png";
            break;
        case CropStage::GROWING_1:
            imagePath += "2.png";
            break;
        case CropStage::GROWING_2:
            imagePath += "3.png";
            break;
        case CropStage::GROWING_3:
        case CropStage::MATURE:
            imagePath += "4.png";
            break;
        case CropStage::DEAD:
            imagePath += "dead.png";
            break;
    }
    cropSprite->setTexture(imagePath);
}

void Crop::handleWeatherChange(float temperature, const std::string& weather) {
    if (temperature < growthConditions.minTemp ||
        temperature > growthConditions.maxTemp) {
        if (currentStage != CropStage::DEAD) {
            currentStage = CropStage::DEAD;
            updateSprite();
        }
    }

    // 下雨时自动浇水
    if (weather == "Rain") {
        isWatered = true;
    }
}

void Crop::handleSeasonChange(const std::string& season) {
    if (season != growthConditions.preferredSeason) {
        totalGrowthTime *= 1.5f; // 非最佳季节生长更慢
    }
}