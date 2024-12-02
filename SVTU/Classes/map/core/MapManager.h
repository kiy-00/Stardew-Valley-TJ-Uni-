#ifndef __MAP_MANAGER_H__
#define __MAP_MANAGER_H__

#include "GameMap.h"
#include "cocos2d.h"
#include <string>
#include <unordered_map>

USING_NS_CC;

class MapManager {
public:
  // 获取单例实例
  static MapManager *getInstance();

  // 加载地图
  bool loadMap(const std::string &mapId);

  // 获取当前地图
  GameMap *getCurrentMap() { return m_currentMap; }

  // 切换地图
  bool switchMap(const std::string &mapId);

  // 保存地图状态
  void saveMapState();

  // 清理资源
  void clearMapResources();

private:
  MapManager() {}; // 私有构造函数
  ~MapManager() {};
  MapManager(const MapManager &) = delete;
  MapManager &operator=(const MapManager &) = delete;

  static MapManager *s_instance;
  GameMap *m_currentMap;
  std::unordered_map<std::string, GameMap *> m_mapCache;
};

#endif