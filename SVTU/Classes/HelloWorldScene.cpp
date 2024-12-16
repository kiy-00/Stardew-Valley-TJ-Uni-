// TestScene.cpp
#include "HelloWorldScene.h"

USING_NS_CC;

// 创建场景
Scene *HelloWorldScene::createScene() { return HelloWorldScene::create(); }

// 初始化方法
bool HelloWorldScene::init() {
  if (!Scene::initWithPhysics()) {
    return false;
  }

  // 设置物理世界的重力为零（因为我们希望精灵在平面上移动）
  this->getPhysicsWorld()->setGravity(Vec2(1, 1));

  // 启用物理调试绘制
  this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

  // 获取屏幕尺寸
  Size visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  // 创建静态精灵（房屋）
  auto houseSprite = Sprite::create("HelloWorld.png");
  if (!houseSprite) {
    CCLOG("无法加载 HelloWorld.png 作为房屋精灵！");
    return false;
  }
  houseSprite->setPosition(Vec2(visibleSize.width / 2 + origin.x,
                                visibleSize.height / 2 + origin.y));
  houseSprite->setScale(1.0f); // 根据需要调整缩放比例

  // 创建物理体积并绑定到房屋精灵
  auto housePhysicsBody = PhysicsBody::createBox(
      houseSprite->getContentSize() * houseSprite->getScale(),
      PhysicsMaterial(1.0f, 0.5f, 0.5f)); // 根据需求调整物理材料参数
  housePhysicsBody->setDynamic(true);     // 静态物体
  housePhysicsBody->setCategoryBitmask(0x01);          // 类别掩码
  housePhysicsBody->setCollisionBitmask(0x02);         // 碰撞掩码
  housePhysicsBody->setContactTestBitmask(0xFFFFFFFF); // 接触测试掩码

  houseSprite->setPhysicsBody(housePhysicsBody);

  // 添加房屋精灵到场景
  this->addChild(houseSprite);
  CCLOG("房屋精灵已创建并添加到场景，位置: (%.2f, %.2f)",
        houseSprite->getPosition().x, houseSprite->getPosition().y);

  // 创建动态精灵（玩家）
  playerSprite = Sprite::create("HelloWorld.png");
  if (!playerSprite) {
    CCLOG("无法加载 HelloWorld.png 作为玩家精灵！");
    return false;
  }
  playerSprite->setPosition(
      Vec2(visibleSize.width / 2 + origin.x,
           visibleSize.height / 2 + origin.y + 200)); // 放置在房屋上方
  playerSprite->setScale(0.5f); // 根据需要调整缩放比例

  // 创建物理体积并绑定到玩家精灵
  auto playerPhysicsBody = PhysicsBody::createBox(
      playerSprite->getContentSize() * playerSprite->getScale(),
      PhysicsMaterial(1.0f, 0.5f, 0.5f)); // 根据需求调整物理材料参数
  playerPhysicsBody->setDynamic(true);    // 动态物体
  playerPhysicsBody->setCategoryBitmask(0x02);          // 类别掩码
  playerPhysicsBody->setCollisionBitmask(0x01);         // 碰撞掩码
  playerPhysicsBody->setContactTestBitmask(0xFFFFFFFF); // 接触测试掩码

  playerSprite->setPhysicsBody(playerPhysicsBody);

  // 添加玩家精灵到场景
  this->addChild(playerSprite);
  CCLOG("玩家精灵已创建并添加到场景，位置: (%.2f, %.2f)",
        playerSprite->getPosition().x, playerSprite->getPosition().y);

  // 设置键盘输入监听
  setupKeyboard();

  // 启用每帧更新（如果需要额外逻辑）
  this->scheduleUpdate();

  return true;
}

// 设置键盘输入
void HelloWorldScene::setupKeyboard() {
  auto keyboardListener = EventListenerKeyboard::create();

  keyboardListener->onKeyPressed =
      CC_CALLBACK_2(HelloWorldScene::onKeyPressed, this);

  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener,
                                                           this);
}

void HelloWorldScene::onKeyPressed(EventKeyboard::KeyCode keyCode,
                                   Event *event) {
  float impulse = 50.0f; // 调整冲量大小
  Vec2 direction = Vec2::ZERO;

  switch (keyCode) {
  case EventKeyboard::KeyCode::KEY_W:
    direction = Vec2(0, 1);
    break;
  case EventKeyboard::KeyCode::KEY_S:
    direction = Vec2(0, -1);
    break;
  case EventKeyboard::KeyCode::KEY_A:
    direction = Vec2(-1, 0);
    break;
  case EventKeyboard::KeyCode::KEY_D:
    direction = Vec2(1, 0);
    break;
  default:
    return;
  }

  direction = direction.getNormalized();
  playerSprite->getPhysicsBody()->applyImpulse(direction * impulse);
  CCLOG("应用冲量: (%f, %f)", direction.x * impulse, direction.y * impulse);
}

// 每帧更新（当前不需要额外逻辑）
void HelloWorldScene::update(float dt) {
  // 可以在这里添加额外的更新逻辑
}
