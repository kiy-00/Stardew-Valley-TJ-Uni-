// MapManager.cpp

#include "MapManager.h"



MapManager *MapManager::instance = nullptr;



MapManager::MapManager()

    : mapLoader(new MapLoader()), mapBuilder(new MapBuilder()),

      cameraFollow(nullptr) {}



MapManager::~MapManager() {

  if (cameraFollow) {

    cameraFollow->release();

  }

}



MapManager *MapManager::getInstance() {

  if (!instance) {

    instance = new MapManager();

  }

  return instance;

}



void MapManager::destroyInstance() {

  if (instance) {

    delete instance;

    instance = nullptr;

  }

}



bool MapManager::init() {

  // 初始化各组件

  if (!mapLoader || !mapBuilder) {

    return false;

  }

  return true;

}



bool MapManager::loadMap(const std::string &tmxPath,

                         MapConstants::MapType type) {

  // 先卸载当前地图

  // unloadCurrentMap();



  // 加载TMX地图

  auto tmxMap = mapLoader->loadTMXMap(tmxPath);

  if (!tmxMap) {

    return false;

  }



  // 构建游戏地图

  currentMap = mapBuilder->buildMap(tmxMap, type);

  if (!currentMap) {

    return false;

  }



  return true;

}



void MapManager::unloadCurrentMap() {

  // 停止相机跟随

  stopCamera();



  // 清理当前地图

  if (currentMap) {

    // // 如果TMX地图存在，从场景中移除

    // if (auto tmxMap = getCurrentTMXMap()) {

    //   tmxMap->removeFromParent();

    // }

    // currentMap.reset();

  }

}



void MapManager::setupCamera(cocos2d::Node *target) {

  if (!currentMap || !target) {

    return;

  }



  auto tmxMap = getCurrentTMXMap();

  if (!tmxMap) {

    return;

  }



  // 停止现有相机跟随

  stopCamera();



  // 设置边界限制

  auto mapSize = tmxMap->getMapSize();

  auto tileSize = tmxMap->getTileSize();

  auto followBoundary = cocos2d::Rect(0, 0, mapSize.width * tileSize.width,

                                      mapSize.height * tileSize.height);



  // 创建新的相机跟随动作

  cameraFollow = cocos2d::Follow::create(target, followBoundary);

  cameraFollow->retain();



  // 让地图执行跟随动作

  tmxMap->runAction(cameraFollow);

}



void MapManager::stopCamera() {

  if (cameraFollow) {

    // if (auto tmxMap = getCurrentTMXMap()) {

    //   tmxMap->stopAction(cameraFollow);

    // }

    cameraFollow->release();

    cameraFollow = nullptr;

  }

}



bool MapManager::isInViewport(const cocos2d::Vec2 &position) const {

  if (!currentMap) {

    return false;

  }



  auto viewport = getViewport();

  return viewport.containsPoint(position);

}



cocos2d::Rect MapManager::getViewport() const {

  // if (!currentMap || !getCurrentTMXMap()) {

  //   return cocos2d::Rect::ZERO;

  // }



  auto tmxMap = getCurrentTMXMap();

  auto pos = tmxMap->getPosition();

  auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();



  return cocos2d::Rect(-pos.x, -pos.y, visibleSize.width, visibleSize.height);

}



// 代理到GameMap的查询方法

// bool MapManager::isWalkable(const cocos2d::Vec2 &position) const {

//   return currentMap ? currentMap->isWalkable(position) : false;

// }



// bool MapManager::hasPhysical(const cocos2d::Vec2 &position) const {

//   return currentMap ? currentMap->hasPhysical(position) : false;

// }



// bool MapManager::isBoundary(const cocos2d::Vec2 &position) const {

//   return currentMap ? currentMap->isBoundary(position) : false;

// }



// bool MapManager::isTransparent(const cocos2d::Vec2 &position) const {

//   return currentMap ? currentMap->isTransparent(position) : false;

// }



cocos2d::TMXTiledMap *MapManager::getCurrentTMXMap() const {

  return currentMap ? currentMap.get() : nullptr;

}