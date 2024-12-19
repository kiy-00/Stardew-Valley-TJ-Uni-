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

  // setupTestMenu();
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

  // 获取窗口大小和地图大小
  Size visibleSize = Director::getInstance()->getVisibleSize();
  Size mapSize = tmxMap->getContentSize();

  // 计算中心位置
  Vec2 centerPosition = Vec2(mapSize.width / 2, mapSize.height / 2);

  playerSprite->setScale(0.2);
  playerSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
  playerSprite->setPosition(centerPosition);
  this->addChild(playerSprite, 999);

  // 设置初始透明度
  updatePlayerVisibility(playerSprite->getPosition());

  // 立即更新摄像机位置
  auto camera = this->getDefaultCamera();
  camera->setPosition(centerPosition);

  CCLOG("Player initialized at position: (%.2f, %.2f)", centerPosition.x,
        centerPosition.y);

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

  // 添加按键按下事件
  listener->onKeyPressed = CC_CALLBACK_2(HelloWorldScene::onKeyPressed, this);

  // 添加按键释放事件
  listener->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event *event) {
    CCLOG("Key released: %d", (int)keyCode);
  };

  // 使用addEventListenerWithFixedPriority替代
  _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);

  CCLOG("键盘事件监听器设置完成");
}

void HelloWorldScene::onKeyPressed(EventKeyboard::KeyCode keyCode,
                                   Event *event) {
  CCLOG("Key pressed: %d", (int)keyCode); // 添加按键日志

  float moveDistance = 10.0f; // 减小移动距离使移动更平滑
  Vec2 currentPos = playerSprite->getPosition();
  Vec2 targetPos = currentPos;

  switch (keyCode) {
  case EventKeyboard::KeyCode::KEY_W:
  case EventKeyboard::KeyCode::KEY_UP_ARROW: // 添加方向键支持
    targetPos.y += moveDistance;
    break;
  case EventKeyboard::KeyCode::KEY_S:
  case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
    targetPos.y -= moveDistance;
    break;
  case EventKeyboard::KeyCode::KEY_A:
  case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
    targetPos.x -= moveDistance;
    break;
  case EventKeyboard::KeyCode::KEY_D:
  case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    targetPos.x += moveDistance;
    break;
  default:
    return;
  }

  // 简化移动逻辑，先不考虑碰撞检测
  playerSprite->setPosition(targetPos);
  updatePlayerVisibility(targetPos);

  // 记录移动日志
  CCLOG("Player moved to: (%.2f, %.2f)", targetPos.x, targetPos.y);
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