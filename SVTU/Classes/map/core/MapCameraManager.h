// MapCameraManager.h
#ifndef __MAP_CAMERA_MANAGER_H__
#define __MAP_CAMERA_MANAGER_H__

#include "cocos2d.h"

USING_NS_CC;

class MapCameraManager {
public:
  static MapCameraManager *getInstance();

  void initWithTarget(Node *target, Node *followNode);
  void updateCamera();

  // 设置边界
  void setBoundary(const Size &mapSize, const Size &viewSize);

  // 获取当前视口范围内的瓦片坐标
  Rect getVisibleTileRange();

private:
  MapCameraManager() {};
  static MapCameraManager *s_instance;

  Node *m_target = nullptr;     // 通常是地图节点
  Node *m_followNode = nullptr; // 通常是玩家节点
  Size m_mapSize;
  Size m_viewSize;

  // 计算摄像机位置
  Vec2 calculateCameraPosition();
  // 确保摄像机在边界内
  Vec2 clampCameraPosition(const Vec2 &position);
};

#endif