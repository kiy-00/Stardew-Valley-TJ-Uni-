﻿#ifndef __PHYSICS_TEST_SCENE_H__

#define __PHYSICS_TEST_SCENE_H__



#include "cocos2d.h"



class PhysicsTestScene : public cocos2d::Scene {

public:

  static cocos2d::Scene *createScene();

  virtual bool init();

  CREATE_FUNC(PhysicsTestScene);



private:

  cocos2d::Sprite *addNewSpriteAtPosition(cocos2d::Vec2 pos);

  bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event);

  bool onContactBegin(cocos2d::PhysicsContact &contact);

};



#endif