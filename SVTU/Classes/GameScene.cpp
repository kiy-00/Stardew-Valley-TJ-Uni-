﻿#include "GameScene.h"
#include "LayerRenderer.h"
#include "LayerRendererFactory.h"

USING_NS_CC;

// 创建场景
Scene *GameScene::createScene() { return GameScene::create(); }

bool GameScene::init() {
  if (!Scene::init()) {
    return false;
  }

  // 直接加载并添加地图到场景
  tmxMap = TMXTiledMap::create("maps/farm/island/island_spring.tmx");
  if (!tmxMap) {
    return false;
  }
  this->addChild(tmxMap);

  // 计算地图居中位置
  // auto visibleSize = Director::getInstance()->getVisibleSize();
  // auto origin = Director::getInstance()->getVisibleOrigin();
  // float x = origin.x + (visibleSize.width - tmxMap->getContentSize().width) /
  // 2; float y =
  //     origin.y + (visibleSize.height - tmxMap->getContentSize().height) / 2;
  // tmxMap->setPosition(Vec2(x, y));

  // 初始化玩家
  if (!initPlayer()) {
    return false;
  }

  loadObjectLayers();

  // // 设置相机
  // setupCamera();

  // 设置键盘输入
  setupKeyboard();

  return true;
}

bool GameScene::initPlayer() {
  player = Sprite::create("HelloWorld.png");
  player->setScale(0.2f);
  if (player) {
    player->setAnchorPoint(Vec2(0, 0));
    player->setPosition(Vec2(800, 800)); // 设置到地图中心位置
  }
  return true;
}

void GameScene::setupCamera() {
  // 获取默认摄像机
  auto camera = this->getDefaultCamera();

  // 设置摄像机的初始位置为玩家的位置
  camera->setPosition(player->getPosition());

  // 启用每帧更新
  this->scheduleUpdate();
}

void GameScene::movePlayerRandomly(float dt) {
  if (!player || !tmxMap)
    return;

  Size mapSizeInTiles = tmxMap->getMapSize();
  Size tileSize = tmxMap->getTileSize();
  float mapPixelWidth = mapSizeInTiles.width * tileSize.width;
  float mapPixelHeight = mapSizeInTiles.height * tileSize.height;

  // 生成随机坐标，确保玩家不会移动到地图外
  float randomX =
      static_cast<float>(std::rand() % static_cast<int>(mapPixelWidth));
  float randomY =
      static_cast<float>(std::rand() % static_cast<int>(mapPixelHeight));
  Vec2 targetPos(randomX, randomY);

  CCLOG("随机移动玩家到: (%.2f, %.2f)", randomX, randomY);

  // 创建移动动作
  auto moveAction = MoveTo::create(2.0f, targetPos);

  // 创建一个回调函数，移动完成后输出玩家的新位置
  auto callback = CallFunc::create([this]() {
    Vec2 newPos = player->getPosition();
    // CCLOG("玩家已移动到: (%.2f, %.2f)", newPos.x, newPos.y);
  });

  // 创建序列动作
  auto sequence = Sequence::create(moveAction, callback, nullptr);
  player->runAction(sequence);
}

void GameScene::update(float dt) {
  Scene::update(dt);

  if (!player || !tmxMap)
    return;

  auto camera = this->getDefaultCamera();

  // 获取地图实际尺寸
  Size mapSize = tmxMap->getContentSize();
  Size visibleSize = Director::getInstance()->getVisibleSize();

  // 获取玩家位置
  Vec2 playerPos = player->getPosition();

  // 限制相机范围
  float x = clampf(playerPos.x, visibleSize.width / 2,
                   mapSize.width - visibleSize.width / 2);
  float y = clampf(playerPos.y, visibleSize.height / 2,
                   mapSize.height - visibleSize.height / 2);

  camera->setPosition(Vec2(x, y));
}

// 自定义绘制方法
void GameScene::draw(Renderer *renderer, const Mat4 &transform,
                     uint32_t flags) {
  Scene::draw(renderer, transform, flags);
  // 其他自定义渲染逻辑
}

void GameScene::setupKeyboard() {
  auto keyboardListener = EventListenerKeyboard::create();

  keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);

  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener,
                                                           this);
}

void GameScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode,
                             cocos2d::Event *event) {
  float moveDistance = 32.0f; // 每次移动的像素数

  Vec2 currentPos = player->getPosition();
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

  // 创建移动动作
  auto moveAction = MoveTo::create(0.2f, targetPos);

  // 创建一个回调函数，移动完成后输出玩家的新位置
  auto callback = CallFunc::create([this]() {
    Vec2 newPos = player->getPosition();
    // CCLOG("玩家已移动到: (%.2f, %.2f)", newPos.x, newPos.y);
  });

  // 创建序列动作
  auto sequence = Sequence::create(moveAction, callback, nullptr);
  player->runAction(sequence);
}

void GameScene::loadObjectLayers() {
  auto map = this->tmxMap;
  if (!map) {
    CCLOG("地图未正确加载。");
    return;
  }

  std::vector<std::string> objectLayers = {"house",    "myhouse",  "obj_fb_2",
                                           "obj_mv_1", "static_1", "static_4",
                                           "static_5"};

  // 获取农场配置并添加错误检查
  auto configManager = FarmConfigManager::getInstance();
  if (!configManager) {
    CCLOG("FarmConfigManager实例获取失败");
    return;
  }

  FarmTypeConfig islandConfig;
  try {
    islandConfig = configManager->getFarmConfig("island");
  } catch (const std::exception &e) {
    CCLOG("获取农场配置失败: %s", e.what());
    return;
  }

  for (const auto &layerName : objectLayers) {
    try {
      CCLOG("开始处理图层: %s", layerName.c_str());

      auto objectGroup = map->getObjectGroup(layerName);
      if (!objectGroup) {
        CCLOG("未找到对象组: %s", layerName.c_str());
        continue;
      }

      // 检查对象组是否为空
      auto objects = objectGroup->getObjects();
      if (objects.empty()) {
        CCLOG("对象组 %s 中没有对象", layerName.c_str());
        continue;
      }

      std::string spritePath = LayerRendererFactory::getSpritePath(layerName);
      if (spritePath.empty()) {
        CCLOG("未找到对象层 '%s' 的精灵路径", layerName.c_str());
        continue;
      }

      // 验证文件是否存在
      if (!FileUtils::getInstance()->isFileExist(spritePath)) {
        CCLOG("精灵文件不存在: %s", spritePath.c_str());
        continue;
      }

      // 检查配置是否存在
      if (islandConfig.layers.objectLayers.find(layerName) ==
          islandConfig.layers.objectLayers.end()) {
        CCLOG("在配置中未找到图层 %s 的属性", layerName.c_str());
        continue;
      }

      // 安全地获取图层属性
      const auto &layerProps = islandConfig.layers.objectLayers.at(layerName);

      CCLOG("准备渲染图层 %s", layerName.c_str());
      CCLOG("对象数量: %zu", objects.size());

      LayerRenderer::renderObjectLayer(objectGroup, map, layerName, spritePath,
                                       layerProps);

      CCLOG("图层 %s 处理完成", layerName.c_str());

    } catch (const std::exception &e) {
      CCLOG("处理图层 %s 时发生错误: %s", layerName.c_str(), e.what());
    } catch (...) {
      CCLOG("处理图层 %s 时发生未知错误", layerName.c_str());
    }
  }

  CCLOG("所有图层处理完成");
}