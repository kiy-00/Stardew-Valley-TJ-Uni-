// FarmMapManager.cpp
#include "FarmMapManager.h"

static FarmMapManager *instance = nullptr;

FarmMapManager *FarmMapManager::getInstance() {
  if (!instance) {
    instance = new FarmMapManager();
  }
  return instance;
}

bool FarmMapManager::initWithFarmType(const std::string &farmType,
                                      const std::string &season) {
  // 获取农场配置
  auto configManager = FarmConfigManager::getInstance();
  if (!configManager) {
    CCLOG("FarmConfigManager实例获取失败");
    return false;
  }

  try {
    // 获取农场配置
    farmConfig = configManager->getFarmConfig(farmType);
    currentFarmType = farmType;

    // 从tmxPath中提取基础路径和文件名
    std::string basePath =
        farmConfig.tmxPath.substr(0, farmConfig.tmxPath.find_last_of("."));
    // 构造带季节的地图路径：如 maps/farm/island_spring.tmx
    std::string mapPath = basePath + "_" + season + ".tmx";

    map = TMXTiledMap::create(mapPath);
    if (!map) {
      CCLOG("地图加载失败: %s", mapPath.c_str());
      return false;
    }

    return true;
  } catch (const std::exception &e) {
    CCLOG("初始化农场失败: %s", e.what());
    return false;
  }
}

Vec2 FarmMapManager::worldToTileCoord(const Vec2 &worldPos) const {
  if (!map)
    return Vec2::ZERO;

  Size tileSize = map->getTileSize();
  return Vec2(worldPos.x / tileSize.width,
              (map->getMapSize().height * tileSize.height - worldPos.y) /
                  tileSize.height);
}

bool FarmMapManager::checkLayerProperty(const Vec2 &worldPos,
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

  // 首先检查对象层
  const auto &objectLayers = farmConfig.layers.objectLayers;
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
        }
      }
    }
  }

  // 然后检查瓦片层
  const auto &tileLayers = farmConfig.layers.tileLayers;
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

  // 最后检查静态对象层
  const auto &staticLayers = farmConfig.layers.staticObjectLayers;
  auto staticLayerIt = staticLayers.find(layerName);
  if (staticLayerIt != staticLayers.end()) {
    auto objectGroup = map->getObjectGroup(layerName);
    if (objectGroup) {
      auto objects = objectGroup->getObjects();
      for (const auto &obj : objects) {
        auto objDict = obj.asValueMap();
        float x = objDict["x"].asFloat();
        float y = objDict["y"].asFloat();
        float width = objDict["width"].asFloat();
        float height = objDict["height"].asFloat();

        Rect objRect(x, y + height, width, height);
        if (objRect.containsPoint(worldPos)) {
          if (property == "physical")
            return staticLayerIt->second.physical;
          if (property == "boundary")
            return staticLayerIt->second.boundary;
          if (property == "base")
            return staticLayerIt->second.base;
          if (property == "penetrable")
            return staticLayerIt->second.penetrable;
        }
      }
    }
  }

  return false;
}

// 地图查询接口实现
bool FarmMapManager::isWalkable(const Vec2 &worldPos) const {
  // 位置可行走：该位置不是物理障碍且不是边界
  for (const auto &[layerName, props] : farmConfig.layers.objectLayers) {
    if (checkLayerProperty(worldPos, layerName, "physical"))
      return false;
    if (checkLayerProperty(worldPos, layerName, "boundary"))
      return false;
  }
  for (const auto &[layerName, props] : farmConfig.layers.tileLayers) {
    if (checkLayerProperty(worldPos, layerName, "physical"))
      return false;
    if (checkLayerProperty(worldPos, layerName, "boundary"))
      return false;
  }
  return true;
}

bool FarmMapManager::isPenetrable(const Vec2 &worldPos) const {
  // 遍历所有图层，检查是否有任何一个图层在该位置标记为可穿透
  for (const auto &[layerName, props] : farmConfig.layers.objectLayers) {
    if (checkLayerProperty(worldPos, layerName, "penetrable"))
      return true;
  }
  for (const auto &[layerName, props] : farmConfig.layers.staticObjectLayers) {
    if (checkLayerProperty(worldPos, layerName, "penetrable"))
      return true;
  }
  return false;
}

bool FarmMapManager::isBoundary(const Vec2 &worldPos) const {
  // 检查所有图层是否有边界属性
  for (const auto &[layerName, props] : farmConfig.layers.tileLayers) {
    if (checkLayerProperty(worldPos, layerName, "boundary"))
      return true;
  }
  return false;
}

bool FarmMapManager::isPhysical(const Vec2 &worldPos) const {
  // 检查所有图层是否有物理属性
  for (const auto &[layerName, props] : farmConfig.layers.objectLayers) {
    if (checkLayerProperty(worldPos, layerName, "physical"))
      return true;
  }
  for (const auto &[layerName, props] : farmConfig.layers.tileLayers) {
    if (checkLayerProperty(worldPos, layerName, "physical"))
      return true;
  }
  return false;
}

bool FarmMapManager::isBase(const Vec2 &worldPos) const {
  // 检查所有图层是否有基础层属性
  for (const auto &[layerName, props] : farmConfig.layers.tileLayers) {
    if (checkLayerProperty(worldPos, layerName, "base"))
      return true;
  }
  return false;
}