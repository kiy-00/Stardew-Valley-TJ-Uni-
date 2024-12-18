// LayerRenderer.h
#pragma once
#include "MapConstants.h"
#include "cocos2d.h"

class LayerRenderer {
public:
  // 渲染普通图层
  static void renderLayer(cocos2d::TMXLayer *layer,
                          const std::string &layerName,
                          const std::string &spritePath,
                          const LayerProperties &layerProps);

  // 渲染对象层
  static void renderObjectLayer(cocos2d::TMXObjectGroup *objectGroup,
                                cocos2d::TMXTiledMap *tmxMap,
                                const std::string &layerName,
                                const std::string &spritePath,
                                const LayerProperties &layerProps);
};
