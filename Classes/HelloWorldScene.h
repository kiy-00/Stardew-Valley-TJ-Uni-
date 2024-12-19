// HelloWorldScene.h
#pragma once

#include "FarmMapManager.h"
#include "cocos2d.h"

class HelloWorldScene : public cocos2d::Scene {
public:
  static cocos2d::Scene *createScene();
  virtual bool init();
  CREATE_FUNC(HelloWorldScene);

private:
  // 地图相关
  cocos2d::TMXTiledMap *tmxMap;
  FarmMapManager *farmMapManager;
  std::string currentMapType;
  std::string currentSeason;

  // 玩家相关
  cocos2d::Sprite *playerSprite;

  // 初始化函数
  bool initMap();
  bool initPlayer();
  void setupTestMenu();
  void setupKeyboard();

  // 事件处理
  void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode,
                    cocos2d::Event *event);
  // void switchSeason(const std::string &season); // 注释掉这行

  // 更新函数
  void update(float dt) override;
  void updatePlayerVisibility(const cocos2d::Vec2 &position);
};