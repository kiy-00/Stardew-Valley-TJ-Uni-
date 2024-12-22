#include "TeleportManager.h"
#include "MapManager.h"
#include "json/document.h"
#include "FileUtils.h"

TeleportManager* TeleportManager::instance = nullptr;

TeleportManager* TeleportManager::getInstance() {
    if (!instance) {
        instance = new TeleportManager();
        instance->init();
    }
    return instance;
}

bool TeleportManager::init() {
    if (!Node::init()) {
        return false;
    }
    return true;
}

void TeleportManager::loadTeleportPoints(const std::string& mapId) {
    teleportPoints.clear();
    
    // 读取farm_types.json
    auto fileContent = FileUtils::getInstance()->getStringFromFile("configs/maps/farm_types.json");
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    
    // 查找对应地图的传送点配置
    if (doc.HasMember("farm_types") && doc["farm_types"].HasMember(mapId.c_str())) {
        const auto& mapConfig = doc["farm_types"][mapId.c_str()];
        if (mapConfig.HasMember("teleport_points")) {
            const auto& points = mapConfig["teleport_points"];
            for (const auto& point : points.GetArray()) {
                TeleportPoint tp;
                tp.targetMap = point["target_map"].GetString();
                tp.position = Vec2(point["position"][0].GetFloat(), 
                                 point["position"][1].GetFloat());
                teleportPoints.push_back(tp);
            }
        }
    }
}

bool TeleportManager::checkTeleport(const Vec2& playerPosition) {
    for (const auto& tp : teleportPoints) {
        if (playerPosition.distance(tp.position) < TELEPORT_TRIGGER_DISTANCE) {
            // 触发传送
            return MapManager::getInstance()->switchMap(tp.targetMap, tp.position);
        }
    }
    return false;
}
