#include "Crop.h"
#include "WeatherSystem.h"
#include "TimeSeasonSystem.h"

Crop* Crop::create(const std::string& type, FarmlandTile* tile) {
    Crop* crop = new (std::nothrow) Crop();
    if (crop && crop->init(type, tile)) {
        crop->autorelease();
        return crop;
    }
    CC_SAFE_DELETE(crop);
    return nullptr;
}


bool Crop::init(const std::string& type, FarmlandTile* tile) {
    if (!Node::init()) return false;

    this->cropType = type;
    this->farmland = tile;
    updateSprite();

    setupEventListeners();

    schedule([this](float dt) {
        this->update(dt);
        }, 1.0f, "crop_update");

    return true;
}

void Crop::setupEventListeners() {
    // 注册天气变化监听
    WeatherSystem::getInstance()->addWeatherChangeListener(
        cropType + "_weather_listener",
        [this](const WeatherChangeEvent& event) {
            std::string newWeather = WeatherSystem::getInstance()->weatherToString(event.newWeather);
            auto it = properties.weatherModifiers.find(newWeather);
            if (it != properties.weatherModifiers.end()) {
                weatherGrowthModifier = it->second;
                CCLOG("Crop %s weather modifier updated to %.2f", cropType.c_str(), weatherGrowthModifier);
            }
        }
    );

    // 注册季节变化监听
    TimeSeasonSystem::getInstance()->addSeasonChangeListener(
        cropType + "_season_listener",
        [this](const TimeSeasonSystem::SeasonChangeEvent& event) {
            std::string newSeason = TimeSeasonSystem::getInstance()->seasonToString(event.newSeason);
            auto it = properties.seasonModifiers.find(newSeason);
            if (it != properties.seasonModifiers.end()) {
                seasonGrowthModifier = it->second;
                CCLOG("Crop %s season modifier updated to %.2f for season %s (Year %d, Day %d)",
                    cropType.c_str(), seasonGrowthModifier, newSeason.c_str(),
                    event.year, event.day);
            }
        }
    );
}

void Crop::removeEventListeners() {
    WeatherSystem::getInstance()->removeWeatherChangeListener(cropType + "_weather_listener");
    TimeSeasonSystem::getInstance()->removeSeasonChangeListener(cropType + "_season_listener");
}

float Crop::calculateGrowthRate() {
    float baseRate = properties.growthRate;

    // 应用季节和天气影响
    float rate = baseRate * seasonGrowthModifier * weatherGrowthModifier;

    // 环境条件检查
    if (!isWatered) rate *= 0.5f;
    if (!isFertilized) rate *= 0.7f;

    return rate;
}

void Crop::update(float dt) {
    if (currentStage == GrowthStage::DEAD) return;

    if (checkEnvironmentConditions()) {
        float growth = calculateGrowthRate() * dt;
        growthProgress += growth;
        checkGrowthStage();
    }
}

void Crop::checkGrowthStage() {
    for (size_t i = 0; i < properties.stageThresholds.size(); ++i) {
        if (growthProgress >= properties.stageThresholds[i]) {
            GrowthStage newStage = static_cast<GrowthStage>(i + 1);
            if (currentStage != newStage) {
                currentStage = newStage;
                updateSprite();
            }
        }
    }
}

void Crop::updateSprite() {
    std::string spritePath;
    switch (currentStage) {
        case GrowthStage::SEED:
            spritePath = "crops/" + cropType + "/seed_" + cropType + ".png";
            break;
        case GrowthStage::STAGE1:
            spritePath = "crops/" + cropType + "/" + cropType + "_1.png";
            break;
        case GrowthStage::STAGE2:
            spritePath = "crops/" + cropType + "/" + cropType + "_2.png";
            break;
        case GrowthStage::STAGE3:
            spritePath = "crops/" + cropType + "/" + cropType + "_3.png";
            break;
        case GrowthStage::DEAD:
            spritePath = "crops/" + cropType + "/" + cropType + "_dead.png";
            break;
    }

    auto sprite = Sprite::create(spritePath);
    if (sprite) {
        this->removeAllChildren();
        this->addChild(sprite);
        sprite->setAnchorPoint(Vec2(0.5f, 0.0f));  // 设置锚点在底部中心
        sprite->setPosition(Vec2(0, 0));
    }
    else {
        CCLOG("Failed to load sprite: %s", spritePath.c_str());
    }
}

bool Crop::checkEnvironmentConditions() {
    if (!farmland) return false;

    // 检查水分和肥力是否满足需求
    isWatered = farmland->getMoisture() >= properties.waterNeed;
    isFertilized = farmland->getFertility() >= properties.fertilityNeed;

    // 如果长期得不到水分或肥料，作物会死亡
    if (!isWatered || !isFertilized) {
        deathCounter++;
        if (deathCounter >= 5) {  // 5天没有满足条件就死亡
            currentStage = GrowthStage::DEAD;
            updateSprite();
            return false;
        }
    }
    else {
        deathCounter = 0;
    }

    return true;
}

bool Crop::canHarvest() const {
    return currentStage == GrowthStage::STAGE3;
}

void Crop::harvest() {
    if (canHarvest()) {
        this->removeFromParent();
    }
}