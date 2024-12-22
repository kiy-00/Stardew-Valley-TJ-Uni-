// InsideMapRenderer.h
#pragma once
#include "cocos2d.h"

USING_NS_CC;

class InsideMapRenderer {
public:
  static InsideMapRenderer *getInstance();

  void renderMap(TMXTiledMap *map);

private:
  InsideMapRenderer() {}

  void renderTileLayers(TMXTiledMap *map);
  void renderObjectLayers(TMXTiledMap *map);
};