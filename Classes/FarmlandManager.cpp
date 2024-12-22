// FarmlandManager.cpp
#include "FarmlandManager.h"
#include "FarmMapManager.h"

FarmlandManager* FarmlandManager::instance = nullptr;



// FarmlandManager.cpp
bool FarmlandManager::init(cocos2d::Scene* scene, const std::vector<Vec2>& farmablePositions, int zOrder) {
    if (scene == nullptr) {
        CCLOG("map does not exists!");
        return false;
    }
    this->farmlandLayer = Layer::create();
    scene->addChild(farmlandLayer, zOrder);  // 使锟矫达拷锟斤拷锟? zOrder
    this->farmablePositions = farmablePositions;

    for (const auto& worldPos : farmablePositions) {
        auto tile = FarmlandTile::create(worldPos);
        if (tile) {
            farmlandLayer->addChild(tile);
            // 将世界坐标转换为瓦片坐标存储
            Vec2 tilePos = FarmMapManager::getInstance()->worldToTileCoord(worldPos);
            std::string key = positionToString(tilePos);
            CCLOG("Storing farmland at tile position: %s (world: %.1f, %.1f)", 
                  key.c_str(), worldPos.x, worldPos.y);
            farmlands[key] = tile;
        }
    }

    farmlandLayer->schedule([this](float dt) {
        this->update(dt);
        }, 1.0f, "farmland_update");

    return true;
}
void FarmlandManager::handleToolAction(const std::string& toolType, const Vec2& tilePos, int direction) {
    CCLOG("FarmlandManager::handleToolAction - Start");
    CCLOG("Tool Type: %s, Tile Position: (%.0f, %.0f)", 
        toolType.c_str(), tilePos.x, tilePos.y);

    auto tile = getFarmlandAt(tilePos);
    if (!tile) {
        CCLOG("No farmland tile found at tile position!");
        return;
    }

    CCLOG("Found farmland tile at position");
    
    if (toolType == "Hoe") {
        CCLOG("Executing till action");
        tile->till();
    }
    else if (toolType == "Kettle") {
        CCLOG("Executing water action");
        tile->water();
    }
    else if (toolType == "Fertilizer") {
        CCLOG("Executing fertilize action");
        tile->fertilize();
    }
    CCLOG("FarmlandManager::handleToolAction - End");
}

FarmlandTile* FarmlandManager::getFarmlandAt(const Vec2& tilePos) {
    CCLOG("Looking for farmland at tile position: %.1f, %.1f", tilePos.x, tilePos.y);
    
    // 在周围3x3范围内查找最近的农田
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            Vec2 nearbyPos(std::round(tilePos.x + dx), std::round(tilePos.y + dy));
            std::string key = positionToString(nearbyPos);
            
            auto it = farmlands.find(key);
            if (it != farmlands.end()) {
                CCLOG("Found farmland tile at nearby position: %s", key.c_str());
                return it->second;
            }
        }
    }

    CCLOG("No farmland found near position: %.1f, %.1f", tilePos.x, tilePos.y);
    return nullptr;
}

// 辅助方法：将位置转换为字符串键值
std::string FarmlandManager::positionToString(const Vec2& pos) {
    return StringUtils::format("%.0f,%.0f", std::round(pos.x), std::round(pos.y));
}

void FarmlandManager::update(float dt) {
    for (auto& pair : farmlands) {
        pair.second->update(dt);
    }
}