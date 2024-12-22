#ifndef INSIDE_MAP_MANAGER_H
#define INSIDE_MAP_MANAGER_H
#include "InsideConfigManager.h"
#include "cocos2d.h"

USING_NS_CC;

class InsideMapManager {
public:
  static InsideMapManager *getInstance();

  bool init();
  TMXTiledMap *getMap() const { return map; }

  // 地图查询接口
  bool isWalkable(const Vec2 &worldPos) const;
  bool isPenetrable(const Vec2 &worldPos) const;
  bool isBoundary(const Vec2 &worldPos) const;
  bool isPhysical(const Vec2 &worldPos) const;
  bool isBase(const Vec2 &worldPos) const;
  bool isInteractive(const Vec2 &worldPos) const;

private:
  InsideMapManager() : map(nullptr) {}

  TMXTiledMap *map;
  InsideConfig insideConfig;

  bool checkLayerProperty(const Vec2 &worldPos, const std::string &layerName,
                          const std::string &property) const;
  Vec2 worldToTileCoord(const Vec2 &worldPos) const;
};
#endif