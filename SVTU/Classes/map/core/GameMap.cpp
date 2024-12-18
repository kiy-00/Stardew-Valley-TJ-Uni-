// GameMap.cpp

#include "GameMap.h"



bool GameMap::init() {

  if (!TMXTiledMap::init()) {

    return false;

  }



  // 初始化地图大小和图块大小

  mapSize = this->getMapSize();

  tileSize = this->getTileSize();



  // 初始化图层

  if (!initLayers()) {

    return false;

  }



  return true;

}



GameMap::~GameMap() {

  // 清理资源（如果有需要）

}



void GameMap::setPlayer(Node *player) { m_player = player; }



void GameMap::update(float dt) {

  // 暂时没有更新逻辑

}



const MapLayer *GameMap::getLayer(const std::string &name) const {

  // 返回 nullptr，简化处理

  return nullptr;

}



std::vector<const MapLayer *>

GameMap::getLayersByProperty(const LayerProperties &props) const {

  // 返回空向量，简化处理

  return {};

}



bool GameMap::isWalkable(const cocos2d::Vec2 &position) const {

  // 简单返回 true，所有位置均可行走

  return true;

}



bool GameMap::hasPhysical(const cocos2d::Vec2 &position) const {

  // 简单返回 false

  return false;

}



bool GameMap::isBoundary(const cocos2d::Vec2 &position) const {

  // 简单边界检测，返回位置是否在地图范围外

  return position.x < 0 || position.y < 0 ||

         position.x >= mapSize.x * tileSize.x ||

         position.y >= mapSize.y * tileSize.y;

}



bool GameMap::isTransparent(const cocos2d::Vec2 &position) const {

  // 简单返回 false

  return false;

}



int GameMap::calculateTileID(const cocos2d::Vec2 &tilePos) const {

  // 返回 0，示例

  return 0;

}



bool GameMap::initLayers() {

  // 简单返回 true，跳过图层初始化

  return true;

}



void GameMap::updateVisibleTiles() {

  // 没有可见图块逻辑

}



bool GameMap::validateMapState() const {

  // 简单返回 true

  return true;

}