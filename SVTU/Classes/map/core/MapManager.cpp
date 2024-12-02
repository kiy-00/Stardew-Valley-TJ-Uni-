#include "MapManager.h"

MapManager *MapManager::s_instance = nullptr;

MapManager *MapManager::getInstance() {
  if (s_instance == nullptr) {
    s_instance = new MapManager();
  }
  return s_instance;
}

bool MapManager::loadMap(const std::string &mapId) {
  // 检查缓存中是否已存在
  auto it = m_mapCache.find(mapId);
  if (it != m_mapCache.end()) {
    m_currentMap = it->second;
    return true;
  }

  // 创建新地图
  auto newMap = new GameMap();
  if (!newMap->initWithTMXFile("maps/" + mapId + ".tmx")) {
    CC_SAFE_DELETE(newMap);
    return false;
  }

  m_mapCache[mapId] = newMap;
  m_currentMap = newMap;
  return true;
}

bool MapManager::switchMap(const std::string &mapId) {
  if (m_currentMap) {
    // 保存当前地图状态
    saveMapState();
  }
  return loadMap(mapId);
}

void MapManager::saveMapState() {
  if (m_currentMap) {
    m_currentMap->saveState();
  }
}

void MapManager::clearMapResources() {
  for (auto &pair : m_mapCache) {
    CC_SAFE_DELETE(pair.second);
  }
  m_mapCache.clear();
  m_currentMap = nullptr;
}