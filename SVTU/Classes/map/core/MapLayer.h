﻿#ifndef __MAP_LAYER_H__

#define __MAP_LAYER_H__



#include "MapConstants.h"

#include "cocos2d.h"



USING_NS_CC;





class MapLayer : public TMXLayer {

protected:

  LayerProperties properties;

  std::string name;

  bool visible;



public:

  CREATE_FUNC(MapLayer);

  virtual bool init() override;



  // 获取图层属性

  const LayerProperties &getProperties() const { return properties; }



  // virtual void loadFromTMX();



  // // 更新图层

  // virtual void update(float dt);



  // // 更新可见区域的瓦片

  // void updateVisibleTiles(const Rect &visibleRange);



  // // 瓦片操作

  // void setTileAt(const Vec2 &pos, int gid);

  // int getTileGIDAt(const Vec2 &pos);



protected:

  // // 更新单个瓦片

  // virtual void updateTileAt(const Vec2 &pos);



private:

};



#endif // __MAP_LAYER_H__