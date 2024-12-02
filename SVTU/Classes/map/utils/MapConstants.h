#ifndef __MAP_CONSTANTS_H__
#define __MAP_CONSTANTS_H__

enum class LayerType {
  TERRAIN,  // 地形层
  BUILDING, // 建筑层
  DECORATE, // 装饰层
  COLLISION // 碰撞层
};

// 地图配置常量
const int TILE_SIZE = 32;          // 瓦片大小
const int DEFAULT_MAP_WIDTH = 50;  // 默认地图宽度
const int DEFAULT_MAP_HEIGHT = 50; // 默认地图高度

#endif