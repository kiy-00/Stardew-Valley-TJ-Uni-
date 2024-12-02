#ifndef __MAP_LAYER_H__
#define __MAP_LAYER_H__

#include "MapConstants.h"
#include "cocos2d.h"

USING_NS_CC;

class MapLayer : public TMXLayer {
public:
  CREATE_FUNC(MapLayer);
  virtual bool init() override;

  // 设置图层类型
  void setLayerType(LayerType type) { m_layerType = type; }
  LayerType getLayerType() const { return m_layerType; }

  // 更新图层
  void update(float dt);

  // 修改瓦片
  void setTileAt(const Vec2 &tilePos, int tileGID);
  int getTileGIDAt(const Vec2 &tilePos);

private:
  LayerType m_layerType;
};

#endif