// HelloWorldScene.h
#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "FarmMapRenderer.h"
#include "cocos2d.h"

class HelloWorldScene : public cocos2d::Scene {
public:
  static cocos2d::Scene *createScene();
  virtual bool init();
  CREATE_FUNC(HelloWorldScene);

private:
  // 地图相关
  cocos2d::TMXTiledMap *tmxMap;
  std::string currentMapType;
  std::string currentSeason;

  // 玩家精灵
  cocos2d::Sprite *playerSprite;

  // 初始化函数
  bool initMap();
  bool initPlayer();

  // 键盘控制
  void setupKeyboard();
  void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode,
                    cocos2d::Event *event);

  // 测试功能
  void setupTestMenu();
  void switchSeason(const std::string &season);

  // 更新
  virtual void update(float dt) override;
};

#endif // __HELLOWORLD_SCENE_H__