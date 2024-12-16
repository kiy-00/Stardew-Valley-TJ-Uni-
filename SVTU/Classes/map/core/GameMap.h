#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__

#include "MapCameraManager.h"
#include "MapConstants.h"
#include "MapLayer.h"
#include "cocos2d.h"
#include <unordered_map>
#include <vector>

class GameMap : public TMXTiledMap {
private:
  std::vector<std::unique_ptr<MapLayer>> layers;
  cocos2d::Vec2 mapSize;
  cocos2d::Vec2 tileSize;

public:
  // 添加默认构造函数
  GameMap() : TMXTiledMap() {}

  CREATE_FUNC(GameMap);
  virtual bool init() override;
  virtual ~GameMap();

  // 玩家和摄像机相关
  void setPlayer(Node *player);
  Node *getPlayer() const { return m_player; }

  // 更新
  virtual void update(float dt) override;

  // 图层查询
  const MapLayer *getLayer(const std::string &name) const;
  std::vector<const MapLayer *>
  getLayersByProperty(const LayerProperties &props) const;

  // 位置查询（考虑所有层的属性）
  bool isWalkable(const cocos2d::Vec2 &position) const;
  bool hasPhysical(const cocos2d::Vec2 &position) const;
  bool isBoundary(const cocos2d::Vec2 &position) const;
  bool isTransparent(const cocos2d::Vec2 &position) const;

  void setMapType(MapConstants::MapType type) { m_mapType = type; }

protected:
  bool initLayers();
  void updateVisibleTiles();
  bool validateMapState() const;

private:
  Node *m_player{nullptr};
  // 内部工具函数
  int calculateTileID(const Vec2 &tilePos) const;

  MapConstants::MapType m_mapType{MapConstants::MapType::NONE};

  // 禁止拷贝
  GameMap(const GameMap &) = delete;
  GameMap &operator=(const GameMap &) = delete;
};

#endif // __GAME_MAP_H__