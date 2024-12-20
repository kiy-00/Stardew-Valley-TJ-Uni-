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
  currentFarmType = farmType;
  currentSeason = season;  // 保存初始季节
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

void FarmMapManager::changeSeason(const std::string &newSeason) {
    currentSeason = newSeason;  // 更新当前季节
    // 释放当前地图资源
    releaseMap();
    // 重新初始化地图
    initWithFarmType(currentFarmType, newSeason);
}

void FarmMapManager::releaseMap() {
    if (map) {
        //map->removeFromParent();
        //map = nullptr;
    }
}

Vec2 FarmMapManager::worldToTileCoord(const Vec2 &worldPos) const {
  if (!map)
    return Vec2::ZERO;

  Size tileSize = map->getTileSize();
  // 修正 X 坐标的计算，减去一个瓦片的宽度
  return Vec2((worldPos.x - tileSize.width) / tileSize.width,
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
        if (property == "arable")
          return tileLayerIt->second.arable;
        if (property == "fishing")
          return tileLayerIt->second.fishing;
        if (property == "farmpermit")
          return tileLayerIt->second.farmpermit;
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
  // 简化移动判定：只要在基础层上有瓦片就可以行走
  for (const auto &[layerName, props] : farmConfig.layers.tileLayers) {
    if (props.base && checkLayerProperty(worldPos, layerName, "base")) {
      return true;
    }
  }
  return false;
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

bool FarmMapManager::isFarmPermit(const Vec2 &worldPos) const {
  for (const auto &[layerName, props] : farmConfig.layers.tileLayers) {
    if (props.farmpermit&& checkLayerProperty(worldPos, layerName, "farmpermit")) {
      return true;
    }
  }
  return false;
}

bool FarmMapManager::isArable(const Vec2& worldPos) const {
    for (const auto& [layerName, props] : farmConfig.layers.tileLayers) {
        if (props.arable && checkLayerProperty(worldPos, layerName, "arable")) {
            return true;
        }
    }
    return false;
}



bool FarmMapManager::isFishingSpot(const Vec2 &worldPos) const {
  if (!map) return false;
  
  Vec2 centerTile = worldToTileCoord(worldPos);
  
  // 检查以当前位置为中心的3x3范围
  for (int offsetX = -1; offsetX <= 1; offsetX++) {
    for (int offsetY = -1; offsetY <= 1; offsetY++) {
      Vec2 checkTile(centerTile.x + offsetX, centerTile.y + offsetY);
      
      // 检查坐标是否在地图范围内
      if (checkTile.x < 0 || checkTile.y < 0 ||
          checkTile.x >= map->getMapSize().width ||
          checkTile.y >= map->getMapSize().height) {
        continue;
      }

      int tileLayerCount = 0;
      TMXLayer* fishingLayer = nullptr;
      
      // 遍历所有瓦片图层
      for (const auto &[layerName, props] : farmConfig.layers.tileLayers) {
        auto layer = map->getLayer(layerName);
        if (layer && layer->getTileGIDAt(checkTile)) {
          tileLayerCount++;
          // 如果这个图层有钓鱼属性，记住它
          if (props.fishing) {
            fishingLayer = layer;
          }
        }
      }
      
      // 如果只有一个图层有瓦片，且是钓鱼图层
      if (tileLayerCount == 1 && fishingLayer != nullptr) {
        return true;
      }
    }
  }
  
  return false;
}