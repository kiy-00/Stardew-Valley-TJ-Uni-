// Classes/scenes/BaseScene.h

#ifndef __BASE_SCENE_H__

#define __BASE_SCENE_H__



#include "cocos2d.h"



USING_NS_CC;



class BaseScene : public Scene {

public:

  static Scene *createScene();

  virtual bool init() override; // 确保声明了虚函数

  CREATE_FUNC(BaseScene);

};



#endif