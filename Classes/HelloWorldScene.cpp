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

    // 初始化交互管理器
    interactionManager = FarmInteractionManager::getInstance();
    interactionManager->init(this, player, farmMapManager);
    
    // 添加更新回调
    this->schedule([this](float dt) {
        interactionManager->update(dt);
    }, 0.1f, "interaction_update");

    // 设置测试
    setupSeasonTest();
    // setupWeatherTest();  // 暂时注释掉天气测试

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

    this->addChild(player, 999);

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

  // 传入当前季节参数
  FarmMapRenderer::getInstance()->renderMap(tmxMap, farmMapManager->getCurrentSeason());

  return true;
}

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

void HelloWorldScene::onSeasonChanged(const std::string &newSeason) {
    // 安全地移除当前地图和相关资源
    if (tmxMap) {
        tmxMap->removeFromParent();
        tmxMap = nullptr;
    }
    // 更新FarmMapManager中的季节
    if (farmMapManager) {
        farmMapManager->changeSeason(newSeason);
    }
    // 重新初始化地图
    if (!initMap()) {
        CCLOG("Failed to re-init map with new season: %s", newSeason.c_str());
        return;
    }
    
    // 通知交互管理器
    interactionManager->onSeasonChanged(newSeason);
}

void HelloWorldScene::setupSeasonTest() {
    currentSeasonIndex = 0;
    
    // 创建季节显示标签
    auto visibleSize = Director::getInstance()->getVisibleSize();
    seasonLabel = Label::createWithTTF("Current Season: Spring", "fonts/Marker Felt.ttf", 24);
    seasonLabel->setPosition(Vec2(visibleSize.width/2, visibleSize.height - 30));
    this->addChild(seasonLabel, 1000);
    
    this->schedule(CC_SCHEDULE_SELECTOR(HelloWorldScene::switchToNextSeason), 4.0f);
}

void HelloWorldScene::switchToNextSeason(float dt) {
    currentSeasonIndex = (currentSeasonIndex + 1) % seasons.size();
    std::string newSeason = seasons[currentSeasonIndex];
    
    // 如果完成一轮循环，回到春季后停止
    if (currentSeasonIndex == 0) {
        this->unschedule(CC_SCHEDULE_SELECTOR(HelloWorldScene::switchToNextSeason));
        CCLOG("Season test completed, returning to spring");
    }
    
    // 更新季节标签
    updateSeasonLabel(newSeason);
    
    // 触发季节切换
    onSeasonChanged(newSeason);
}

void HelloWorldScene::updateSeasonLabel(const std::string& season) {
    std::string text = "Current Season: " + season;
    seasonLabel->setString(text);
}

void HelloWorldScene::setupWeatherTest() {
    currentWeatherIndex = 0;
    
    // 创建天气显示标签
    auto visibleSize = Director::getInstance()->getVisibleSize();
    weatherLabel = Label::createWithTTF("Current Weather: Sunny", "fonts/Marker Felt.ttf", 24);
    weatherLabel->setPosition(Vec2(visibleSize.width/2, visibleSize.height - 60));
    this->addChild(weatherLabel, 1000);
    
    // 设置定时器，每20秒切换一次天气
    this->schedule(CC_SCHEDULE_SELECTOR(HelloWorldScene::switchToNextWeather), 20.0f);
    
    // 初始通知
    interactionManager->onWeatherChanged("sunny");
}

void HelloWorldScene::switchToNextWeather(float dt) {
    currentWeatherIndex = (currentWeatherIndex + 1) % weathers.size();
    std::string newWeather = weathers[currentWeatherIndex];
    
    // 更新标签
    weatherLabel->setString("Current Weather: " + newWeather);
    
    // 通知交互管理器
    interactionManager->onWeatherChanged(newWeather);
}