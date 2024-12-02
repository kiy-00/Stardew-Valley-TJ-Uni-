#include "BaseScene.h"

bool BaseScene::init() {
  if (!Scene::init()) {
    return false;
  }
  return true;
}