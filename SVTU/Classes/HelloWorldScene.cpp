// HelloWorldScene.cpp
#include "HelloWorldScene.h"
#include "FarmMapManager.h"
#include "FarmMapRenderer.h"
#include "SpritePathManager.h"

USING_NS_CC;

Scene *HelloWorldScene::createScene() { return HelloWorldScene::create(); }

bool HelloWorldScene::init() {
  if (!Scene::init()) {
    return false;
  }

  // 更新初始化参数
  std::string farmMapType = "island";
  std::string season = "spring";

  // 初始化地图管理器
  farmMapManager = FarmMapManager::getInstance();
  if (!farmMapManager->initWithFarmType(farmMapType, season)) {
    CCLOG("Failed to init FarmMapManager");
    return false;
  }

  // 初始化其他组件
  if (!initMap() || !initPlayer()) {
    return false;
  }

  setupTestMenu();
  setupKeyboard();
  this->scheduleUpdate();

  return true;
}

bool HelloWorldScene::initMap() {
  // 直接从 FarmMapManager 获取地图
  tmxMap = farmMapManager->getMap();
  if (!tmxMap) {
    CCLOG("Failed to get map from FarmMapManager");
    return false;
  }
  this->addChild(tmxMap);

  // 使用渲染器渲染地图
  FarmMapRenderer::getInstance()->renderMap(tmxMap);

  return true;
}

bool HelloWorldScene::initPlayer() {
  playerSprite = Sprite::create("HelloWorld.png");
  if (!playerSprite) {
    return false;
  }

  playerSprite->setScale(0.2f);
  playerSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
  playerSprite->setPosition(Vec2(800, 800));
  tmxMap->addChild(playerSprite, 999); // 确保玩家在最上层

  // 设置初始透明度
  updatePlayerVisibility(playerSprite->getPosition());

  return true;
}

// 删除季节切换按钮
void HelloWorldScene::setupTestMenu() {
  // 空实现,不再需要切换按钮
}

/* 注释掉季节切换函数
void HelloWorldScene::switchSeason(const std::string &season) {
    // ...existing code...
}
*/

void HelloWorldScene::setupKeyboard() {
  auto listener = EventListenerKeyboard::create();
  listener->onKeyPressed = CC_CALLBACK_2(HelloWorldScene::onKeyPressed, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void HelloWorldScene::onKeyPressed(EventKeyboard::KeyCode keyCode,
                                   Event *event) {
  float moveDistance = 32.0f;
  Vec2 currentPos = playerSprite->getPosition();
  Vec2 targetPos = currentPos;

  switch (keyCode) {
  case EventKeyboard::KeyCode::KEY_W:
    targetPos.y += moveDistance;
    break;
  case EventKeyboard::KeyCode::KEY_S:
    targetPos.y -= moveDistance;
    break;
  case EventKeyboard::KeyCode::KEY_A:
    targetPos.x -= moveDistance;
    break;
  case EventKeyboard::KeyCode::KEY_D:
    targetPos.x += moveDistance;
    break;
  default:
    return;
  }

  // 检查目标位置是否可移动
  if (farmMapManager->isWalkable(targetPos) ||
      farmMapManager->isPenetrable(targetPos)) {
    auto moveAction = MoveTo::create(0.2f, targetPos);
    auto callback = CallFunc::create(
        [this, targetPos]() { updatePlayerVisibility(targetPos); });
    auto sequence = Sequence::create(moveAction, callback, nullptr);
    playerSprite->runAction(sequence);
  }
}

void HelloWorldScene::updatePlayerVisibility(const Vec2 &position) {
  if (farmMapManager->isPenetrable(position)) {
    // 在可穿透区域设置半透明
    playerSprite->setOpacity(128);
  } else {
    // 在普通区域恢复完全不透明
    playerSprite->setOpacity(255);
  }
}

void HelloWorldScene::update(float dt) {
  if (!playerSprite || !tmxMap)
    return;

  // 更新相机跟随玩家
  auto camera = this->getDefaultCamera();
  Size mapSize = tmxMap->getContentSize();
  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 playerPos = playerSprite->getPosition();

  float x = clampf(playerPos.x, visibleSize.width / 2,
                   mapSize.width - visibleSize.width / 2);
  float y = clampf(playerPos.y, visibleSize.height / 2,
                   mapSize.height - visibleSize.height / 2);

  camera->setPosition(Vec2(x, y));
}