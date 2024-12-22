// InsideMapManager.cpp
#include "InsideMapManager.h"

static InsideMapManager *instance = nullptr;

InsideMapManager *InsideMapManager::getInstance() {
  if (!instance) {
    instance = new InsideMapManager();
  }
  return instance;
}

bool InsideMapManager::init() {
  // 获取室内配置
  auto configManager = InsideConfigManager::getInstance();
  if (!configManager) {
    CCLOG("InsideConfigManager实例获取失败");
    return false;
  }

  try {
    // 获取室内配置
    insideConfig = configManager->getInsideConfig();

    map = TMXTiledMap::create(insideConfig.tmxPath);
    if (!map) {
      CCLOG("地图加载失败: %s", insideConfig.tmxPath.c_str());
      return false;
    }

    return true;
  } catch (const std::exception &e) {
    CCLOG("初始化室内地图失败: %s", e.what());
    return false;
  }
}

Vec2 InsideMapManager::worldToTileCoord(const Vec2 &worldPos) const {
  if (!map)
    return Vec2::ZERO;

  Size tileSize = map->getTileSize();
  return Vec2(worldPos.x / tileSize.width,
              (map->getMapSize().height * tileSize.height - worldPos.y) /
                  tileSize.height);
}

bool InsideMapManager::checkLayerProperty(const Vec2 &worldPos,
                                          const std::string &layerName,
                                          const std::string &property) const {
  if (!map)
    return false;

  // 转换为瓦片坐标
  Vec2 tileCoord = worldToTileCoord(worldPos);

  // 检查坐标是否在地图范围内
  if (tileCoord.x < 0 || tileCoord.y < 0 ||
      tileCoord.x >= map->getMapSize().width ||
      tileCoord.y >= map->getMapSize().height) {
    return false;
  }

  // 检查对象层
  const auto &objectLayers = insideConfig.layers.objectLayers;
  auto objLayerIt = objectLayers.find(layerName);
  if (objLayerIt != objectLayers.end()) {
    auto objectGroup = map->getObjectGroup(layerName);
    if (objectGroup) {
      auto objects = objectGroup->getObjects();
      for (const auto &obj : objects) {
        auto objDict = obj.asValueMap();
        float x = objDict["x"].asFloat();
        float y = objDict["y"].asFloat();
        float width = objDict["width"].asFloat();
        float height = objDict["height"].asFloat();

        // 检查点是否在对象范围内
        Rect objRect(x, y + height, width, height);
        if (objRect.containsPoint(worldPos)) {
          if (property == "physical")
            return objLayerIt->second.physical;
          if (property == "boundary")
            return objLayerIt->second.boundary;
          if (property == "base")
            return objLayerIt->second.base;
          if (property == "penetrable")
            return objLayerIt->second.penetrable;
          if (property == "can_interactive")
            return objLayerIt->second.canInteractive;
        }
      }
    }
  }

  // 检查瓦片层
  const auto &tileLayers = insideConfig.layers.tileLayers;
  auto tileLayerIt = tileLayers.find(layerName);
  if (tileLayerIt != tileLayers.end()) {
    auto layer = map->getLayer(layerName);
    if (layer) {
      // 检查该位置是否有瓦片
      if (layer->getTileGIDAt(tileCoord)) {
        if (property == "physical")
          return tileLayerIt->second.physical;
        if (property == "boundary")
          return tileLayerIt->second.boundary;
        if (property == "base")
          return tileLayerIt->second.base;
        if (property == "penetrable")
          return tileLayerIt->second.penetrable;
      }
    }
  }

  return false;
}

// 地图查询接口实现
bool InsideMapManager::isWalkable(const Vec2 &worldPos) const {
  for (const auto &[layerName, props] : insideConfig.layers.tileLayers) {
    if (props.base && checkLayerProperty(worldPos, layerName, "base")) {
      return true;
    }
  }
  return false;
}

bool InsideMapManager::isPenetrable(const Vec2 &worldPos) const {
  for (const auto &[layerName, props] : insideConfig.layers.objectLayers) {
    if (checkLayerProperty(worldPos, layerName, "penetrable"))
      return true;
  }
  return false;
}

bool InsideMapManager::isBoundary(const Vec2 &worldPos) const {
  for (const auto &[layerName, props] : insideConfig.layers.tileLayers) {
    if (checkLayerProperty(worldPos, layerName, "boundary"))
      return true;
  }
  return false;
}

bool InsideMapManager::isPhysical(const Vec2 &worldPos) const {
  for (const auto &[layerName, props] : insideConfig.layers.objectLayers) {
    if (checkLayerProperty(worldPos, layerName, "physical"))
      return true;
  }
  for (const auto &[layerName, props] : insideConfig.layers.tileLayers) {
    if (checkLayerProperty(worldPos, layerName, "physical"))
      return true;
  }
  return false;
}

bool InsideMapManager::isBase(const Vec2 &worldPos) const {
  for (const auto &[layerName, props] : insideConfig.layers.tileLayers) {
    if (checkLayerProperty(worldPos, layerName, "base"))
      return true;
  }
  return false;
}

bool InsideMapManager::isInteractive(const Vec2 &worldPos) const {
  for (const auto &[layerName, props] : insideConfig.layers.objectLayers) {
    if (checkLayerProperty(worldPos, layerName, "can_interactive")) {
      return true;
    }
  }
  return false;
}