// MapBuilder.h
#include "FarmConfigManager.h"
#include "GameMap.h"
#include "MapConstants.h"
#include "cocos2d.h"

class MapBuilder {
public:
  // 主构建函数
  std::unique_ptr<GameMap> buildMap(cocos2d::TMXTiledMap *tmxMap,
                                    MapConstants::MapType type);

private:
  // 图层解析和构建
  bool parseMapLayers(GameMap *gameMap, cocos2d::TMXTiledMap *tmxMap);
  bool parseTileLayer(GameMap *gameMap, cocos2d::TMXLayer *layer);
  // bool parseObjectGroup(GameMap *gameMap, cocos2d::TMXObjectGroup *objGroup);

  // 属性解析
  LayerProperties parseLayerProperties(const cocos2d::ValueMap &properties);

  // 特定类型地图的构建
  bool buildFarmMap(GameMap *gameMap, cocos2d::TMXTiledMap *tmxMap);
  bool buildPlazaMap(GameMap *gameMap, cocos2d::TMXTiledMap *tmxMap);
  bool buildShopMap(GameMap *gameMap, cocos2d::TMXTiledMap *tmxMap);
  bool buildHomesteadMap(GameMap *gameMap, cocos2d::TMXTiledMap *tmxMap);
};