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
    // 创建用户角色
    player = User::create("Player", "male", 100, 100, 1000);
    if (!player) {
        return false;
    }

    Size mapSize = tmxMap->getContentSize();
    Vec2 centerPosition = Vec2(mapSize.width / 2, mapSize.height / 2);

   
    player->setAnchorPoint(Vec2(0.5f, 0.5f));
    player->setPosition(centerPosition);

    // 初始化物理系统
    player->initPhysics();

    tmxMap->addChild(player, 999);

    // 初始化相机
    auto camera = this->getDefaultCamera();
    camera->setPosition(centerPosition);

    CCLOG("Player initialized at position: (%.2f, %.2f)", centerPosition.x, centerPosition.y);

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

void HelloWorldScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
    float moveDistance = 32.0f;
    Vec2 currentPos = player->getPosition();
    Vec2 targetPos = currentPos;

    switch (keyCode) {
        case EventKeyboard::KeyCode::KEY_W:
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
            targetPos.y += moveDistance;
            player->moveUp();
            break;
        case EventKeyboard::KeyCode::KEY_S:
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            targetPos.y -= moveDistance;
            player->moveDown();
            break;
        case EventKeyboard::KeyCode::KEY_A:
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            targetPos.x -= moveDistance;
            player->moveLeft();
            break;
        case EventKeyboard::KeyCode::KEY_D:
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            targetPos.x += moveDistance;
            player->moveRight();
            break;
        case EventKeyboard::KeyCode::KEY_I:
            player->toggleInventory();
            break;
        default:
            return;
    }

    bool canWalk = farmMapManager->isWalkable(targetPos);
    if (canWalk) {
        auto moveAction = MoveTo::create(0.2f, targetPos);
        auto callback = CallFunc::create([this, targetPos]() {
            bool isPenetrable = farmMapManager->isPenetrable(targetPos);
            player->updateVisibility(isPenetrable);
            player->stopMoving();
            });
        auto sequence = Sequence::create(moveAction, callback, nullptr);
        player->runAction(sequence);
    }
}

void HelloWorldScene::update(float dt) {
    if (!player || !tmxMap)
        return;

    // 更新相机跟随玩家
    auto camera = this->getDefaultCamera();
    Size mapSize = tmxMap->getContentSize();
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 playerPos = player->getPosition();

    float x = clampf(playerPos.x, visibleSize.width / 2,
        mapSize.width - visibleSize.width / 2);
    float y = clampf(playerPos.y, visibleSize.height / 2,
        mapSize.height - visibleSize.height / 2);

    camera->setPosition(Vec2(x, y));
}