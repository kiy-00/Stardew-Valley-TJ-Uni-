﻿// MapBuilder.cpp

#include "MapBuilder.h"



std::unique_ptr<GameMap> MapBuilder::buildMap(cocos2d::TMXTiledMap *tmxMap,

                                              MapConstants::MapType type) {

  if (!tmxMap)

    return std::unique_ptr<GameMap>(nullptr);



  // 创建并初始化 GameMap

  std::unique_ptr<GameMap> gameMap(new GameMap());

  if (!gameMap->initWithTMXFile("maps/farm/island.tmx")) {

    return std::unique_ptr<GameMap>(nullptr);

  }



  return gameMap;

}



bool MapBuilder::parseMapLayers(GameMap *gameMap,

                                cocos2d::TMXTiledMap *tmxMap) {

  if (!gameMap || !tmxMap)

    return false;



  return true;

}



bool MapBuilder::parseTileLayer(GameMap *gameMap, cocos2d::TMXLayer *layer) {

  if (!gameMap || !layer)

    return false;



  // 获取图层属性

  auto properties = parseLayerProperties(layer->getProperties());



  // 创建地图图层

  std::unique_ptr<MapLayer> mapLayer(new MapLayer());

  mapLayer->setName(layer->getLayerName());

  mapLayer->setVisible(layer->isVisible());



  return true;

}



LayerProperties

MapBuilder::parseLayerProperties(const cocos2d::ValueMap &properties) {

  LayerProperties props;



  // 设置默认值

  props.base = false;

  props.boundary = false;

  props.physical = false;

  props.penetrable = false;



  // 从properties中读取属性

  if (properties.find("base") != properties.end()) {

    props.base = properties.at("base").asBool();

  }

  if (properties.find("boundary") != properties.end()) {

    props.boundary = properties.at("boundary").asBool();

  }

  if (properties.find("physical") != properties.end()) {

    props.physical = properties.at("physical").asBool();

  }

  if (properties.find("penetrable") != properties.end()) {

    props.penetrable = properties.at("penetrable").asBool();

  }



  return props;

}



bool MapBuilder::buildFarmMap(GameMap *gameMap, cocos2d::TMXTiledMap *tmxMap) {

  if (!gameMap || !tmxMap)

    return false;



  return true;

}



bool MapBuilder::buildPlazaMap(GameMap *gameMap, cocos2d::TMXTiledMap *tmxMap) {

  return true;

}



bool MapBuilder::buildShopMap(GameMap *gameMap, cocos2d::TMXTiledMap *tmxMap) {

  return true;

}



bool MapBuilder::buildHomesteadMap(GameMap *gameMap,

                                   cocos2d::TMXTiledMap *tmxMap) {

  return true;

}