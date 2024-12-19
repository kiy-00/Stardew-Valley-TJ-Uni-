// HelloWorldScene.cpp
#include "HelloWorldScene.h"
#include "FarmMapManager.h"
#include "FarmMapRenderer.h"
#include "SpritePathManager.h"

USING_NS_CC;

Scene *HelloWorldScene::createScene() { return HelloWorldScene::create(); }

bool HelloWorldScene::init() {
    if (!Scene::initWithPhysics()) {
        return false;
    }

    // 获取物理世界并设置
    auto physicsWorld = this->getPhysicsWorld();
    physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);  // 开启调试绘制
    physicsWorld->setGravity(Vec2::ZERO);  // 设置重力为0

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

    setupKeyboard();
    this->scheduleUpdate();

    return true;
}

// 修改 initPlayer 中的物理体设置
bool HelloWorldScene::initPlayer() {
    playerSprite = Sprite::create("HelloWorld.png");
    if (!playerSprite) {
        return false;
    }
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Size mapSize = tmxMap->getContentSize();
    Vec2 centerPosition = Vec2(mapSize.width / 2, mapSize.height / 2);

    playerSprite->setScale(0.2);
    playerSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    playerSprite->setPosition(centerPosition);

    // 创建略小于精灵显示大小的物理体
    Size playerSize = Size(playerSprite->getContentSize().width,
        playerSprite->getContentSize().height);
    auto physicsBody = PhysicsBody::createBox(
        playerSize,
        PhysicsMaterial(1.0f, 0.0f, 1.0f)
    );

    // 重新设置物理体属性
    physicsBody->setDynamic(true);
    physicsBody->setRotationEnable(false);
    physicsBody->setGravityEnable(false);
    //physicsBody->setMass(1.0f);               // 设置质量
    //physicsBody->setLinearDamping(0.8f);      // 添加阻尼

    // 调整碰撞掩码
    physicsBody->setCategoryBitmask(0xFFFFFFFF);    // 所有类别
    physicsBody->setCollisionBitmask(0xFFFFFFFF);   // 与所有类别碰撞
    physicsBody->setContactTestBitmask(0xFFFFFFFF); // 检测所有碰撞
    // 确保物理体不会进入睡眠状态
    physicsBody->setEnabled(true);
    physicsBody->setTag(1);  // 给物理体设置标签

    playerSprite->setPhysicsBody(physicsBody);

    tmxMap->addChild(playerSprite, 999);

    updatePlayerVisibility(playerSprite->getPosition());

    auto camera = this->getDefaultCamera();
    camera->setPosition(centerPosition);

    CCLOG("Player initialized at position: (%.2f, %.2f)", centerPosition.x,
        centerPosition.y);

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
  CCLOG("Key pressed: %d", (int)keyCode);

  float moveDistance = 32.0f; // 使用瓦片大小作为移动单位
  Vec2 currentPos = playerSprite->getPosition();
  Vec2 targetPos = currentPos;

  switch (keyCode) {
  case EventKeyboard::KeyCode::KEY_W:
  case EventKeyboard::KeyCode::KEY_UP_ARROW:
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

  // 检查目标位置是否可行走
  bool canWalk = farmMapManager->isWalkable(targetPos);
  CCLOG("Attempting to move to (%.2f, %.2f), canWalk: %d", targetPos.x,
        targetPos.y, canWalk);

  if (canWalk) {
    auto moveAction = MoveTo::create(0.2f, targetPos);
    auto callback = CallFunc::create(
        [this, targetPos]() { updatePlayerVisibility(targetPos); });
    auto sequence = Sequence::create(moveAction, callback, nullptr);
    playerSprite->runAction(sequence);
  } else {
    CCLOG("Cannot walk to target position - no base tile found");
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