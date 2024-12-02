// Classes/scenes/GameScene.h
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "BaseScene.h"
#include "map/core/MapManager.h"

class GameScene : public BaseScene {
public:
  static cocos2d::Scene *createScene();
  virtual bool init() override;
  CREATE_FUNC(GameScene);

private:
  void initMap();
};

#endif