// FarmMapRenderer.h
#pragma once
#include "cocos2d.h"

USING_NS_CC;

class FarmMapRenderer {
public:
  static FarmMapRenderer *getInstance();

  void renderMap(TMXTiledMap *map);
  void updateSeason(const std::string &season);

private:
  FarmMapRenderer() {}

  void renderTileLayers(TMXTiledMap *map);
  void renderObjectLayers(TMXTiledMap *map);
  void renderStaticObjectLayers(TMXTiledMap *map);
};