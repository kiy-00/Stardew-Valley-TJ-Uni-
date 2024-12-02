// Classes/scenes/GameScene.cpp
#include "GameScene.h"

Scene *GameScene::createScene() { return GameScene::create(); }

bool GameScene::init() {
  if (!BaseScene::init()) {
    return false;
  }

  initMap();
  return true;
}

void GameScene::initMap() {
  auto mapManager = MapManager::getInstance();

  // 测试地图加载
  if (mapManager->loadMap("test_map")) {
    auto currentMap = mapManager->getCurrentMap();
    this->addChild(currentMap);

    // 临时添加一些测试用的UI
    auto label =
        Label::createWithTTF("Test Map Loaded", "fonts/Marker Felt.ttf", 24);
    label->setPosition(
        Vec2(Director::getInstance()->getVisibleSize().width / 2, 50));
    this->addChild(label);
  }
}