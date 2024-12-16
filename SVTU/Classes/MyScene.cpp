#include "MyScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene *MyScene::createScene() { return MyScene::create(); }

bool MyScene::init() {
  // 初始化物理世界
  if (!Scene::initWithPhysics()) {
    return false;
  }

  // 设置物理世界的重力为零
  this->getPhysicsWorld()->setGravity(Vec2(0, 0));

  // 启用物理调试绘制
  this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

  Size winSize = Director::getInstance()->getWinSize();

  // 创建边界盒子并设置位置为屏幕中心
  auto edgeBody =
      PhysicsBody::createEdgeBox(winSize, PHYSICSBODY_MATERIAL_DEFAULT, 3);
  auto edgeNode = Node::create();
  edgeNode->setPhysicsBody(edgeBody);
  edgeNode->setPosition(winSize.width / 2, winSize.height / 2);
  this->addChild(edgeNode);

  // 注册碰撞监听
  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin =
      CC_CALLBACK_1(MyScene::onContactBegin, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener,
                                                           this);

  // 创建第一个精灵
  auto sprite1 = Sprite::create("HelloWorld.png");
  sprite1->setPosition(Vec2(winSize.width / 2 - 100, winSize.height / 2));
  this->addChild(sprite1);

  auto moveBy1 = MoveBy::create(2, Vec2(500, 0));
  auto moveBy2 = MoveBy::create(2, Vec2(-500, 0));
  sprite1->runAction(
      RepeatForever::create(Sequence::create(moveBy1, moveBy2, nullptr)));

  auto boxBody1 = PhysicsBody::createBox(sprite1->getContentSize());
  boxBody1->setDynamic(true);
  boxBody1->setCategoryBitmask(0x01);    // 类别掩码
  boxBody1->setCollisionBitmask(0x02);   // 碰撞掩码
  boxBody1->setContactTestBitmask(0xFF); // 接触测试掩码设为全部
  sprite1->setPhysicsBody(boxBody1);
  boxBody1->setTag(100);

  // 重要: 设置速度为0,让物理引擎来处理运动
  boxBody1->setVelocity(Vec2::ZERO);

  // 创建第二个精灵
  auto sprite2 = Sprite::create("HelloWorld.png");
  sprite2->setScale(0.5f);
  sprite2->setPosition(Vec2(winSize.width / 2 + 100, winSize.height / 2));
  this->addChild(sprite2);

  auto moveBy3 = MoveBy::create(2, Vec2(-500, 0));
  auto moveBy4 = MoveBy::create(2, Vec2(500, 0));
  sprite2->runAction(
      RepeatForever::create(Sequence::create(moveBy3, moveBy4, nullptr)));

  auto boxBody2 =
      PhysicsBody::createBox(sprite2->getContentSize() * 0.5f); // 注意缩放
  boxBody2->setDynamic(true);
  boxBody2->setCategoryBitmask(0x02);    // 类别掩码
  boxBody2->setCollisionBitmask(0x01);   // 碰撞掩码
  boxBody2->setContactTestBitmask(0xFF); // 接触测试掩码设为全部
  sprite2->setPhysicsBody(boxBody2);
  boxBody2->setTag(200);

  // 重要: 设置速度为0,让物理引擎来处理运动
  boxBody2->setVelocity(Vec2::ZERO);

  return true;
}

bool MyScene::onContactBegin(cocos2d::PhysicsContact &contact) {
  static int collisionCount = 0;
  CCLOG("碰撞次数: %d", collisionCount);
  collisionCount += 1;

  auto bodyA = contact.getShapeA()->getBody();
  auto bodyB = contact.getShapeB()->getBody();

  CCLOG("碰撞对象标签: %d 与 %d", bodyA->getTag(), bodyB->getTag());

  return true;
}