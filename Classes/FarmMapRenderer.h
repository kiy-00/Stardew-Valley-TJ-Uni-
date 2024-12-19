// FarmMapRenderer.h
#pragma once
#include "cocos2d.h"

USING_NS_CC;

class FarmMapRenderer {
public:
  static FarmMapRenderer *getInstance();

  void renderMap(TMXTiledMap *map, const std::string &season);
  void updateSeason(const std::string &season);

private:
  FarmMapRenderer() {}

  void renderTileLayers(TMXTiledMap *map, const std::string &season);
  void renderObjectLayers(TMXTiledMap *map, const std::string &season);
  void renderStaticObjectLayers(TMXTiledMap *map, const std::string &season);
};