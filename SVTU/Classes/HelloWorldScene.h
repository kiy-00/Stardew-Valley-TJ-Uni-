// TestScene.h

#ifndef __HELLOWORLD_SCENE_H__

#define __HELLOWORLD_SCENE_H__



#include "cocos2d.h"



class HelloWorldScene : public cocos2d::Scene {

public:

  // 创建场景

  static cocos2d::Scene *createScene();



  // 初始化方法

  virtual bool init();



  // 实现 create() 方法

  CREATE_FUNC(HelloWorldScene);



private:

  // 动态精灵（玩家）

  cocos2d::Sprite *playerSprite;



  // 处理键盘输入

  void setupKeyboard();

  void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode,

                    cocos2d::Event *event);



  // 更新方法（如果需要）

  virtual void update(float dt) override;

};



#endif // __HELLOWORLD_SCENE_H__

