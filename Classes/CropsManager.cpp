#include "CropsManager.h"
#include "FarmMapManager.h"
#include "FarmConfigManager.h"
#include "FarmlandManager.h"

CropsManager* CropsManager::instance = nullptr;

CropsManager* CropsManager::getInstance() {
    if (!instance) {
        instance = new CropsManager();
    }
    return instance;
}

bool CropsManager::init(cocos2d::Scene* scene, const std::string& farmType,
    const std::string& season, int zOrder) {
    if (scene == nullptr) {
        CCLOG("Scene does not exist!");
        return false;
    }

    currentFarmType = farmType;
    currentSeason = season;

    this->cropsLayer = Layer::create();
    scene->addChild(cropsLayer, zOrder);

    cropsLayer->schedule([this](float dt) {
        this->update(dt);
        }, 1.0f, "crops_update");

    return true;
}

void CropsManager::plantCrop(const std::string& cropType, const Vec2& worldPos) {
    CCLOG("CropsManager::plantCrop - Start");
    CCLOG("Crop Type: %s, World Position: (%.1f, %.1f)",
        cropType.c_str(), worldPos.x, worldPos.y);

    if (!checkPlantingConditions(cropType, worldPos)) {
        return;
    }

    Vec2 tilePos = FarmMapManager::getInstance()->worldToTileCoord(worldPos);
    std::string key = positionToString(tilePos);

    // 创建新作物
    auto crop = Crop::create(cropType);
    if (crop) {
        crop->setPosition(worldPos);
        cropsLayer->addChild(crop);
        crops[key] = crop;

        CCLOG("Successfully planted %s at position: %s",
            cropType.c_str(), key.c_str());
    }

    CCLOG("CropsManager::plantCrop - End");
}

bool CropsManager::checkPlantingConditions(const std::string& cropType, const Vec2& worldPos) const {
    // 1. 检查是否在当前农场和季节可以种植此作物
    if (!isCropAvailable(cropType)) {
        CCLOG("This crop cannot be planted in current farm/season!");
        return false;
    }

    // 2. 检查位置是否可耕种
    if (!FarmMapManager::getInstance()->isArable(worldPos)) {
        CCLOG("Position is not arable!");
        return false;
    }

    // 3. 检查是否已有作物
    Vec2 tilePos = FarmMapManager::getInstance()->worldToTileCoord(worldPos);
    std::string key = CropsManager::positionToString(tilePos);
    if (crops.find(key) != crops.end()) {
        CCLOG("Crop already exists at this position!");
        return false;
    }

    // 4. 检查是否有已开垦的土地
    auto farmland = FarmlandManager::getInstance()->getFarmlandAt(tilePos);
    if (!farmland || !farmland->isTilled()) {
        CCLOG("No tilled farmland at this position!");
        return false;
    }

    return true;
}

void CropsManager::harvestCrop(const Vec2& worldPos) {
    CCLOG("CropsManager::harvestCrop - Start");

    auto crop = getCropAt(worldPos);
    if (!crop || !crop->isFullyGrown()) {
        CCLOG("No harvestable crop at this position!");
        return;
    }

    // 在这里添加收获逻辑，比如添加到玩家背包

    // 移除作物
    removeCrop(worldPos);

    CCLOG("CropsManager::harvestCrop - End");
}

void CropsManager::removeCrop(const Vec2& worldPos) {
    Vec2 tilePos = FarmMapManager::getInstance()->worldToTileCoord(worldPos);
    std::string key = positionToString(tilePos);

    auto it = crops.find(key);
    if (it != crops.end()) {
        it->second->removeFromParent();
        crops.erase(it);
    }
}

Crop* CropsManager::getCropAt(const Vec2& worldPos) {
    Vec2 tilePos = FarmMapManager::getInstance()->worldToTileCoord(worldPos);
    std::string key = positionToString(tilePos);
    auto it = crops.find(key);
    return (it != crops.end()) ? it->second : nullptr;
}

bool CropsManager::isCropAvailable(const std::string& cropType) const {
    auto availableCrops = FarmConfigManager::getInstance()->getAvailableCrops(currentFarmType, currentSeason);
    return std::find(availableCrops.begin(), availableCrops.end(), cropType) != availableCrops.end();
}

void CropsManager::handleToolAction(const std::string& toolType, const Vec2& worldPos, int direction) {
    CCLOG("CropsManager::handleToolAction - Start");

    auto crop = getCropAt(worldPos);
    if (!crop) {
        CCLOG("No crop found at this position!");
        return;
    }

    if (toolType == "Sickle") {
        if (crop->isFullyGrown()) {
            harvestCrop(worldPos);
        }
    }
    else if (toolType == "Wateringcan") {
        crop->water();
    }
    else if (toolType == "Fertilizer") {
        crop->fertilize();
    }

    CCLOG("CropsManager::handleToolAction - End");
}

void CropsManager::handleWeatherChange(float temperature, const std::string& weather) {
    for (auto& pair : crops) {
        pair.second->handleWeatherChange(temperature, weather);
    }
}

void CropsManager::handleSeasonChange(const std::string& season) {
    currentSeason = season;

    // 检查每个作物在新季节是否可以继续生长
    std::vector<Vec2> cropsToRemove;

    for (const auto& pair : crops) {
        auto cropType = pair.second->getCropType();
        if (!isCropAvailable(cropType)) {
            // 如果作物在新季节不能生长，记录其位置以便移除
            Vec2 pos = pair.second->getPosition();
            cropsToRemove.push_back(pos);
        }

        // 通知作物季节变化
        pair.second->handleSeasonChange(season);
    }

    // 移除不能在新季节生长的作物
    for (const auto& pos : cropsToRemove) {
        removeCrop(pos);
    }
}

std::vector<std::string> CropsManager::getAvailableCrops() const {
    return FarmConfigManager::getInstance()->getAvailableCrops(currentFarmType, currentSeason);
}

std::string CropsManager::positionToString(const Vec2& pos) {
    return StringUtils::format("%.0f,%.0f", std::round(pos.x), std::round(pos.y));
}

void CropsManager::update(float dt) {
    // 作物的更新已经在各自的定时器中处理
    // 这里可以添加额外的管理逻辑
}