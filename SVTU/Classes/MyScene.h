#ifndef __MY_SCENE_H__
#define __MY_SCENE_H__

#include "cocos2d.h"

class MyScene : public cocos2d::Scene {
public:
  static cocos2d::Scene *createScene();
  virtual bool init();
  CREATE_FUNC(MyScene);

  bool onContactBegin(cocos2d::PhysicsContact &contact);
};

#endif // __MY_SCENE_H__
