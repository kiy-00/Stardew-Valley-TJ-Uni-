#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__

#include "MapConstants.h"
#include "MapLayer.h"
#include "cocos2d.h"
#include <vector>

USING_NS_CC;

class GameMap : public TMXTiledMap {
public:
  CREATE_FUNC(GameMap);
  virtual bool init() override;

  // 添加新图层
  MapLayer *addLayer(LayerType type);

  // 获取指定类型的图层
  MapLayer *getLayer(LayerType type);

  // 更新地图
  void update(float dt);

  // 保存状态
  virtual void saveState(); // 确保声明了虚函数

  // 地块操作
  bool isTileWalkable(const Vec2 &tilePos);
  void setTileProperty(const Vec2 &tilePos, const std::string &property,
                       bool value);

private:
  std::vector<MapLayer *> m_layers;
  cocos2d::Size m_mapSize;

  bool initLayers();
};

#endif