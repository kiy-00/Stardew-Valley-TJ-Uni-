#pragma once
#include "cocos2d.h"

struct TeleportPoint {
    std::string targetMap;
    cocos2d::Vec2 position;
};

class TeleportManager : public cocos2d::Node {
public:
    static TeleportManager* getInstance();
    
    bool init() override;
    void loadTeleportPoints(const std::string& mapId);
    bool checkTeleport(const cocos2d::Vec2& playerPosition);

private:
    TeleportManager() {}
    static TeleportManager* instance;
    
    std::vector<TeleportPoint> teleportPoints;
    const float TELEPORT_TRIGGER_DISTANCE = 32.0f; // 触发距离
};
