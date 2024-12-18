#pragma once

#include "cocos2d.h"

#include <memory>



class GameObject {

public:

  virtual void render() = 0;

  virtual cocos2d::Sprite *getSprite() = 0;

  virtual ~GameObject() {}

};

class BasicObject : public GameObject {

protected:

  cocos2d::Sprite *sprite;



public:

  BasicObject(cocos2d::Sprite *spr) : sprite(spr) {}



  void render() override {

    // 基本渲染逻辑

    // sprite->setPosition(sprite->getPosition());

  }



  cocos2d::Sprite *getSprite() override { return sprite; }



  ~BasicObject() override {}

};



class GameObjectDecorator : public GameObject {

protected:

  GameObject *decoratedObject; // 改为原始指针



public:

  GameObjectDecorator(GameObject *obj) : decoratedObject(obj) {} // 使用原始指针



  void render() override {

    if (decoratedObject) {

      decoratedObject->render();

    }

  }



  cocos2d::Sprite *getSprite() override {

    return decoratedObject ? decoratedObject->getSprite() : nullptr;

  }



  virtual ~GameObjectDecorator() {

    delete decoratedObject; // 手动释放内存

  }

};



// 添加物理体积

class PhysicsAdder : public GameObjectDecorator {

public:

  // 添加支持原始指针的构造函数

  PhysicsAdder(GameObject *obj) : GameObjectDecorator(obj) {

    auto sprite = getSprite();

    if (sprite) {

      CCLOG("PhysicsAdder: 精灵存在，名称: %s", sprite->getName().c_str());

      if (!sprite->getPhysicsBody()) {

        // 创建一个静态的物理体积

        auto physicsBody = cocos2d::PhysicsBody::createBox(

            sprite->getContentSize(),

            cocos2d::PhysicsMaterial(0.0f, 1.0f, 0.0f));

        physicsBody->setDynamic(true);                  // 静态物体

        physicsBody->setCategoryBitmask(0x02);          // 类别掩码

        physicsBody->setCollisionBitmask(0xFFFFFFFF);   // 与玩家碰撞

        physicsBody->setContactTestBitmask(0xFFFFFFFF); // 接触测试掩码



        // 将物理体积添加到精灵

        sprite->setPhysicsBody(physicsBody);



        CCLOG("为精灵添加了物理体积，尺寸: (%.2f, %.2f)",

              sprite->getContentSize().width, sprite->getContentSize().height);

      } else {

        CCLOG("精灵已经有物理体积，名称: %s", sprite->getName().c_str());

      }

    } else {

      CCLOG("PhysicsAdder: 精灵不存在，无法添加物理体积。");

    }

  }



  void render() override {

    GameObjectDecorator::render();

    // 其他渲染逻辑（如果需要）

  }

};



// 添加边界

class BoundaryAdder : public GameObjectDecorator {

public:

  // 添加支持原始指针的构造函数

  BoundaryAdder(GameObject *obj) : GameObjectDecorator(obj) {

    auto sprite = getSprite();

    if (sprite) {

      auto debugRect = cocos2d::DrawNode::create();

      cocos2d::Size size = sprite->getContentSize();

      cocos2d::Vec2 vertices[4] = {cocos2d::Vec2(0, 0),

                                   cocos2d::Vec2(size.width, 0),

                                   cocos2d::Vec2(size.width, size.height),

                                   cocos2d::Vec2(0, size.height)};

      debugRect->drawRect(vertices[0], vertices[2], cocos2d::Color4F::RED);

      sprite->addChild(debugRect);

    }

  }



  void render() override {

    GameObjectDecorator::render();

    // 其他渲染逻辑（如果需要）

  }

};

