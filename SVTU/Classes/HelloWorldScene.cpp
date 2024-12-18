// HelloWorldScene.cpp
#include "HelloWorldScene.h"
#include "FarmMapRenderer.h"
#include "SpritePathManager.h"

USING_NS_CC;

Scene *HelloWorldScene::createScene() { return HelloWorldScene::create(); }

bool HelloWorldScene::init() {
  if (!Scene::init()) {
    return false;
  }

  currentMapType = "island";
  currentSeason = "spring";

  // 初始化地图
  if (!initMap()) {
    return false;
  }

  // 初始化玩家
  if (!initPlayer()) {
    return false;
  }

  // 设置测试菜单
  setupTestMenu();

  // 设置键盘控制
  setupKeyboard();

  // 开启更新
  this->scheduleUpdate();

  return true;
}

bool HelloWorldScene::initMap() {
  // 加载地图
  tmxMap = TMXTiledMap::create("maps/farm/island/island_spring.tmx");
  if (!tmxMap) {
    CCLOG("Failed to load map");
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

  return true;
}

void HelloWorldScene::setupTestMenu() {
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  // 创建季节切换按钮
  std::vector<std::string> seasons = {"spring", "summer", "fall", "winter"};
  float buttonY = origin.y + visibleSize.height - 30;
  float buttonX = origin.x + 100;

  for (const auto &season : seasons) {
    auto button = MenuItemLabel::create(
        Label::createWithTTF(season, "fonts/arial.ttf", 20),
        [this, season](Ref *sender) { switchSeason(season); });
    button->setPosition(Vec2(buttonX, buttonY));
    buttonX += 100;

    auto menu = Menu::create(button, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
  }
}

void HelloWorldScene::switchSeason(const std::string &season) {
  if (currentSeason == season)
    return;

  currentSeason = season;

  // 更新地图和渲染器
  std::string mapPath = "maps/farm/island/island_" + season + ".tmx";
  auto newMap = TMXTiledMap::create(mapPath);
  if (newMap) {
    // 保存原地图信息
    Vec2 oldPos = tmxMap->getPosition();
    Vec2 playerPos = playerSprite->getPosition();

    // 替换地图
    this->removeChild(tmxMap);
    this->addChild(newMap);
    newMap->setPosition(oldPos);
    tmxMap = newMap;

    // 重新渲染地图
    FarmMapRenderer::getInstance()->renderMap(tmxMap);

    // 恢复玩家位置
    playerSprite->removeFromParent();
    tmxMap->addChild(playerSprite, 999);
    playerSprite->setPosition(playerPos);
  }
}

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

  auto moveAction = MoveTo::create(0.2f, targetPos);
  playerSprite->runAction(moveAction);
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