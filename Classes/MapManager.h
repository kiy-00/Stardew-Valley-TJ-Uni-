#pragma once
#include "cocos2d.h"

class MapManager {
public:
    static MapManager* getInstance();
    
    bool loadMap(const std::string& mapId);
    bool switchMap(const std::string& targetMapId, const cocos2d::Vec2& position);
    
    // 获取当前地图
    cocos2d::TMXTiledMap* getCurrentMap() const { return currentMap; }
    std::string getCurrentMapId() const { return currentMapId; }

private:
    MapManager() : currentMap(nullptr) {}
    
    static MapManager* instance;
    cocos2d::TMXTiledMap* currentMap;
    std::string currentMapId;
    
    std::string getMapPath(const std::string& mapId);
    void cleanupCurrentMap();
};
