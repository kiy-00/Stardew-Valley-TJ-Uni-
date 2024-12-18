#include "PhysicsTestScene.h"
#include "chipmunk/include/chipmunk/chipmunk.h"

void testChipmunk() {
  // 直接调用 chipmunk API
  cpSpace *space = cpSpaceNew();
  CCLOG("Chipmunk Space created: %p", space);

  // 创建一个物体
  cpBody *body = cpBodyNew(1.0f, 1.0f);
  CCLOG("Chipmunk Body created: %p", body);

  // 获取重力
  cpVect gravity = cpSpaceGetGravity(space);
  CCLOG("Gravity: %f, %f", gravity.x, gravity.y);

  // 清理
  cpSpaceFree(space);
  cpBodyFree(body);
}

USING_NS_CC;

Scene *PhysicsTestScene::createScene() { return PhysicsTestScene::create(); }

bool PhysicsTestScene::init() {
  if (!Scene::initWithPhysics()) {
    return false;
  }

  testChipmunk();

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  // 设置更大的重力并打印确认
  this->getPhysicsWorld()->setGravity(Vec2(0, -2000.0f));
  CCLOG("Physics world created with gravity: %f",
        this->getPhysicsWorld()->getGravity().y);

  // 设置物理世界速度
  this->getPhysicsWorld()->setSpeed(1.0f);

  // 开启调试绘制
  this->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

  // 创建边界并使其可见
  auto edgeNode = Node::create();
  auto edgeBody =
      PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.1f, 1.0f, 0.5f),
                                 3.0f // 增加边框宽度使其可见
      );
  edgeNode->setPosition(Vec2(visibleSize.width / 2 + origin.x,
                             visibleSize.height / 2 + origin.y));
  edgeNode->setPhysicsBody(edgeBody);

  // 添加边框可视化
  auto borderDraw = DrawNode::create();
  borderDraw->drawRect(Vec2(-visibleSize.width / 2, -visibleSize.height / 2),
                       Vec2(visibleSize.width / 2, visibleSize.height / 2),
                       Color4F::GREEN);
  edgeNode->addChild(borderDraw);

  this->addChild(edgeNode);

  // 添加标签显示调试信息
  auto label = Label::createWithTTF("Touch screen to add objects",
                                    "fonts/Marker Felt.ttf", 24);
  label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 50));
  this->addChild(label);

  // 添加初始的方块，设置不同的初始速度
  for (int i = 0; i < 5; i++) {
    float x = visibleSize.width * (0.2f + 0.15f * i);
    float y = visibleSize.height * 0.8f;
    auto sprite = addNewSpriteAtPosition(Vec2(x, y));

    // 给每个方块一个初始速度
    auto body = sprite->getPhysicsBody();
    if (body) {
      float vx = (i - 2) * 200.0f; // 不同的水平速度
      body->setVelocity(Vec2(vx, 0));
      body->setAngularVelocity((i - 2) * 2.0f); // 添加旋转
      CCLOG("Created sprite %d with velocity (%f, 0)", i, vx);
    }
  }

  // 添加触摸监听
  auto touchListener = EventListenerTouchOneByOne::create();
  touchListener->onTouchBegan =
      CC_CALLBACK_2(PhysicsTestScene::onTouchBegan, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

  // 添加碰撞监听
  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin =
      CC_CALLBACK_1(PhysicsTestScene::onContactBegin, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener,
                                                           this);

  // 添加更新回调来打印物理信息
  this->schedule(
      [this](float dt) {
        auto bodies = this->getPhysicsWorld()->getAllBodies();
        for (auto body : bodies) {
          auto vel = body->getVelocity();
          if (vel.length() > 0.1f) {
            // CCLOG("Body velocity: (%f, %f)", vel.x, vel.y);
          }
        }
      },
      1.0f, "physics_debug");

  return true;
}

Sprite *PhysicsTestScene::addNewSpriteAtPosition(Vec2 pos) {
  auto sprite = Sprite::create("HelloWorld.png");
  sprite->setScale(0.3f);

  auto body = PhysicsBody::createBox(sprite->getContentSize() * 0.3f,
                                     PhysicsMaterial(0.1f, 1.0f, 0.5f));
  body->setDynamic(true);
  body->setGravityEnable(true);  // 确保重力已启用
  body->setMass(1.0f);           // 设置质量
  body->setRotationEnable(true); // 允许旋转

  sprite->setPhysicsBody(body);
  sprite->setPosition(pos);

  this->addChild(sprite);
  CCLOG("Added new sprite at position (%f, %f)", pos.x, pos.y);

  return sprite;
}

bool PhysicsTestScene::onTouchBegan(Touch *touch, Event *event) {
  auto location = touch->getLocation();
  auto sprite = addNewSpriteAtPosition(location);

  // 给新创建的方块一个随机的初始速度
  auto body = sprite->getPhysicsBody();
  if (body) {
    float vx = (rand() % 400) - 200.0f;
    float vy = rand() % 200;
    body->setVelocity(Vec2(vx, vy));
    CCLOG("Created sprite at touch point with velocity (%f, %f)", vx, vy);
  }

  return true;
}

bool PhysicsTestScene::onContactBegin(PhysicsContact &contact) {
  auto bodyA = contact.getShapeA()->getBody();
  auto bodyB = contact.getShapeB()->getBody();

  Vec2 velA = bodyA->getVelocity();
  Vec2 velB = bodyB->getVelocity();

  CCLOG("Collision detected! Velocities: A(%f, %f) B(%f, %f)", velA.x, velA.y,
        velB.x, velB.y);

  return true;
}