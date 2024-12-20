// FarmScene.cpp
#include "FarmScene.h"
#include "FarmMapManager.h"
#include "FarmMapRenderer.h"
#include "SpritePathManager.h"

USING_NS_CC;

Scene* FarmScene::createScene(const std::string& mapType) {
    return FarmScene::create(mapType);
}

bool FarmScene::init(const std::string& mapType) {
    if (!Scene::initWithPhysics()) {
        return false;
    }

    // 获取物理世界并设置
    auto physicsWorld = this->getPhysicsWorld();
    physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);  // 开启调试绘制
    physicsWorld->setGravity(Vec2::ZERO);  // 设置重力为0

    currentMapType = mapType;  // 保存地图类型
    std::string season = "spring";
    // 初始化地图管理器
    farmMapManager = FarmMapManager::getInstance();
    if (!farmMapManager->initWithFarmType(mapType, season)) {
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

    // 添加到 init() 函数中, 在 setupKeyboard() 之前:
    initInventory();
    setupMouse();

    // 添加到 init() 最后, return true 之前:
    this->scheduleOnce([this](float dt) {
        player->createInventoryBar();
        }, 0.1f, "createInventoryBarKey");

    return true;
}

// 修改 initPlayer 中的物理体设置
bool FarmScene::initPlayer() {
    // 创建用户角色
    player = User::create("Player", "male", 100, 100, 1000);
    if (!player) {
        return false;
    }

    Size mapSize = tmxMap->getContentSize();
    Vec2 centerPosition = Vec2(mapSize.width / 2, mapSize.height / 2);

   
    player->setAnchorPoint(Vec2(0.5, 0));
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

bool FarmScene::initMap() {
  // 直接从 FarmMapManager 获取地图
  tmxMap = farmMapManager->getMap();
  if (!tmxMap) {
    CCLOG("Failed to get map from FarmMapManager");
    return false;
  }
  this->addChild(tmxMap);

  // 传入当前季节参数
  FarmMapRenderer::getInstance()->renderMap(tmxMap, farmMapManager->getCurrentSeason(), currentMapType);

  return true;
}

void FarmScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
    float moveDistance = 16.0f;
    Vec2 currentPos = player->getPosition();
    Vec2 targetPos = currentPos;
    int index = -1;

    switch (keyCode) {
        // 数字键选择物品
        case EventKeyboard::KeyCode::KEY_1:
        case EventKeyboard::KeyCode::KEY_2:
        case EventKeyboard::KeyCode::KEY_3:
        case EventKeyboard::KeyCode::KEY_4:
        case EventKeyboard::KeyCode::KEY_5:
        case EventKeyboard::KeyCode::KEY_6:
        case EventKeyboard::KeyCode::KEY_7:
        case EventKeyboard::KeyCode::KEY_8:
            index = static_cast<int>(keyCode) - static_cast<int>(EventKeyboard::KeyCode::KEY_1);
            player->setSelectedSlot(0, index);
            player->selectItemFromInventory(index);
            break;

            // 移动控制
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

            // 背包和物品控制
        case EventKeyboard::KeyCode::KEY_E:
            if (!player->getIsSlotImageOpen()) {
                player->toggleInventory();
            }
            break;
        case EventKeyboard::KeyCode::KEY_R:
            player->reduceSelectedItemQuantity(1, true);
            break;
        case EventKeyboard::KeyCode::KEY_N:
            if (!player->getIsInventoryOpen()) {
                player->toggleSlotImage();
            }
            break;
        default:
            return;
    }

    // 处理移动逻辑
    if (keyCode == EventKeyboard::KeyCode::KEY_W ||
        keyCode == EventKeyboard::KeyCode::KEY_S ||
        keyCode == EventKeyboard::KeyCode::KEY_A ||
        keyCode == EventKeyboard::KeyCode::KEY_D ||
        keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW ||
        keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW ||
        keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
        keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {

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
}

void FarmScene::update(float dt) {
    if (!player || !tmxMap)
        return;
    // 在现有的 update() 函数中的相机更新前添加:
    player->updateAnimation(dt);

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

void FarmScene::onSeasonChanged(const std::string &newSeason) {
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

void FarmScene::setupSeasonTest() {
    currentSeasonIndex = 0;
    
    // 创建季节显示标签
    auto visibleSize = Director::getInstance()->getVisibleSize();
    seasonLabel = Label::createWithTTF("Current Season: Spring", "fonts/Marker Felt.ttf", 24);
    seasonLabel->setPosition(Vec2(visibleSize.width/2, visibleSize.height - 30));
    this->addChild(seasonLabel, 1000);
    
    this->schedule(CC_SCHEDULE_SELECTOR(FarmScene::switchToNextSeason), 4.0f);
}

void FarmScene::switchToNextSeason(float dt) {
    currentSeasonIndex = (currentSeasonIndex + 1) % seasons.size();
    std::string newSeason = seasons[currentSeasonIndex];
    
    // 如果完成一轮循环，回到春季后停止
    if (currentSeasonIndex == 0) {
        this->unschedule(CC_SCHEDULE_SELECTOR(FarmScene::switchToNextSeason));
        CCLOG("Season test completed, returning to spring");
    }
    
    // 更新季节标签
    updateSeasonLabel(newSeason);
    
    // 触发季节切换
    onSeasonChanged(newSeason);
}

void FarmScene::updateSeasonLabel(const std::string& season) {
    std::string text = "Current Season: " + season;
    seasonLabel->setString(text);
}

void FarmScene::setupWeatherTest() {
    currentWeatherIndex = 0;
    
    // 创建天气显示标签
    auto visibleSize = Director::getInstance()->getVisibleSize();
    weatherLabel = Label::createWithTTF("Current Weather: Sunny", "fonts/Marker Felt.ttf", 24);
    weatherLabel->setPosition(Vec2(visibleSize.width/2, visibleSize.height - 60));
    this->addChild(weatherLabel, 1000);
    
    // 设置定时器，每20秒切换一次天气
    this->schedule(CC_SCHEDULE_SELECTOR(FarmScene::switchToNextWeather), 20.0f);
    
    // 初始通知
    interactionManager->onWeatherChanged("sunny");
}

void FarmScene::switchToNextWeather(float dt) {
    currentWeatherIndex = (currentWeatherIndex + 1) % weathers.size();
    std::string newWeather = weathers[currentWeatherIndex];
    
    // 更新标签
    weatherLabel->setString("Current Weather: " + newWeather);
    
    // 通知交互管理器
    interactionManager->onWeatherChanged(newWeather);
}

void FarmScene::initInventory() {
    ToolItem* reap = new ToolItem("Reap", "A useful tool.", "tool/reap.png");
    player->getInventory()->addItem(reap);
    ToolItem* chop = new ToolItem("Chop", "A useful tool.", "tool/chop.png");
    player->getInventory()->addItem(chop);
    ToolItem* hoe = new ToolItem("Hoe", "A useful tool.", "tool/hoe.png");
    player->getInventory()->addItem(hoe);
    ToolItem* pickaxe = new ToolItem("Pickaxe", "A useful tool.", "tool/pickaxe.png");
    player->getInventory()->addItem(pickaxe);

    // ... 添加更多工具 ...

    player->getInventory()->reduceItemQuantity(0, 0, 2);
}

void FarmScene::setupMouse() {
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(FarmScene::onMouseClick, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

void FarmScene::onMouseClick(EventMouse* event) {
    float slotSize = 30.0f;
    Vec2 mousePosition = event->getLocation();

    float startX = (Director::getInstance()->getVisibleSize().width - (8 * slotSize)) / 2;
    float startY = (Director::getInstance()->getVisibleSize().height - (3 * slotSize)) / 2;

    int row = (startY + (3 * slotSize) - mousePosition.y) / slotSize;
    int col = (mousePosition.x - startX) / slotSize;

    if (mousePosition.x >= startX && mousePosition.x <= startX + (8 * slotSize) &&
        mousePosition.y >= startY && mousePosition.y <= startY + (3 * slotSize)) {
        if (row >= 0 && row < ROWS && col >= 0 && col < SLOTS_PER_ROW) {
            player->onSlotClicked(row, col);
        }
    }
    else if (row == 0 && col == SLOTS_PER_ROW) {
        player->onSlotClicked(0, SLOTS_PER_ROW);
    }
    else {
        if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
            player->performToolAction();
        }
    }
}

void FarmScene::setupKeyboard() {
    auto keyboardListener = EventListenerKeyboard::create();

    // 添加按键按下事件
    keyboardListener->onKeyPressed = CC_CALLBACK_2(FarmScene::onKeyPressed, this);

    // 添加按键释放事件
    keyboardListener->onKeyReleased = CC_CALLBACK_2(FarmScene::onKeyReleased, this);

    // 使用addEventListenerWithFixedPriority替代
    _eventDispatcher->addEventListenerWithFixedPriority(keyboardListener, 1);

    CCLOG("键盘事件监听器设置完成");
}

void FarmScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
    switch (keyCode) {
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case EventKeyboard::KeyCode::KEY_W:
        case EventKeyboard::KeyCode::KEY_A:
        case EventKeyboard::KeyCode::KEY_S:
        case EventKeyboard::KeyCode::KEY_D:
            player->setMoving(false);
            break;
        default:
            break;
    }
}
