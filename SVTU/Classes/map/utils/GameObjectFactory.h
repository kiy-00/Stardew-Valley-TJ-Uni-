#pragma once
#include "GameObject.h"

class GameObjectFactory {
public:
  static GameObject *createGameObject(cocos2d::Sprite *sprite,
                                      const cocos2d::ValueMap &properties);
};