// FarmScene.cpp
#include "FarmScene.h"
#include "FarmMapManager.h"
#include "FarmMapRenderer.h"
#include "SpritePathManager.h"
#include "TimeSeasonSystem.h"

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
    physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    physicsWorld->setGravity(Vec2::ZERO);

    currentMapType = mapType;

    // 初始化时间季节系统 - 修改这部分
    timeSystem = TimeSeasonSystem::getInstance();
    // 一个季节大约10秒的时间流速
    timeSystem->setTimeScale(67.2f);  // 修改时间流速
    // 设置季节变化回调
    timeSystem->setSeasonChangedCallback([this](const std::string& newSeason) {
        CCLOG("Season change callback triggered with season: %s", newSeason.c_str());
        this->onSeasonChanged(newSeason);
    });
    // 立即启动时间系统
    timeSystem->startTime();
    // 立即更新一次标签
    updateTimeSeasonLabel();

    // 提高更新频率
    this->schedule([this](float dt) {
        this->updateTimeSeasonLabel();
        }, 0.1f, "time_update");  // 从0.5秒改为0.1秒

    // 创建时间和季节显示标签 - 修改这部分
    auto visibleSize = Director::getInstance()->getVisibleSize();
    timeSeasonLabel = Label::createWithTTF("Loading...", "fonts/Marker Felt.ttf", 24);
    if (!timeSeasonLabel) {
        CCLOG("Failed to create timeSeasonLabel");
        return false;
    }
    timeSeasonLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 30));
    this->addChild(timeSeasonLabel, 1000);


    // 初始化地图管理器
    farmMapManager = FarmMapManager::getInstance();
    if (!farmMapManager->initWithFarmType(mapType, timeSystem->getCurrentSeasonString())) {
        CCLOG("Failed to init FarmMapManager");
        return false;
    }

    // 初始化其他组件
    if (!initMap() || !initPlayer()) {
        return false;
    }

    setupKeyboard();
    setupMouse();
    initInventory();

    // 初始化交互管理器
    interactionManager = FarmInteractionManager::getInstance();
    interactionManager->init(this, player, farmMapManager);

    // 添加交互更新回调
    this->schedule([this](float dt) {
        interactionManager->update(dt);
        }, 0.1f, "interaction_update");

    this->scheduleUpdate();

    // 创建物品栏
    this->scheduleOnce([this](float dt) {
        player->createInventoryBar();
        }, 0.1f, "createInventoryBarKey");

    // 启动时间系统
    timeSystem->startTime();

    return true;
}

void FarmScene::updateTimeSeasonLabel() {
    // 添加空指针检查
    if (!timeSeasonLabel) {
        CCLOG("Warning: timeSeasonLabel is nullptr");
        return;
    }

    if (!timeSystem) {
        CCLOG("Warning: timeSystem is nullptr");
        return;
    }

    auto time = timeSystem->getCurrentTime();
    std::string seasonStr = timeSystem->getCurrentSeasonString();

    char timeText[100];
    snprintf(timeText, sizeof(timeText),
        "Year %d, %s Day %d\n%02d:%02d",
        time.year,
        seasonStr.c_str(),
        time.day,
        time.hour,
        time.minute
    );

    // 使用 runOnMainThread 确保在主线程中更新UI
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=]() {
        if (timeSeasonLabel) {
            timeSeasonLabel->setString(timeText);
        }
    });

    // 添加调试日志
    CCLOG("Time Update - Year: %d, Season: %s, Day: %d, Time: %02d:%02d",
        time.year, seasonStr.c_str(), time.day, time.hour, time.minute);
}

bool FarmScene::initMap() {
    tmxMap = farmMapManager->getMap();
    if (!tmxMap) {
        CCLOG("Failed to get map from FarmMapManager");
        return false;
    }
    this->addChild(tmxMap);

    FarmMapRenderer::getInstance()->renderMap(
        tmxMap,
        timeSystem->getCurrentSeasonString(),
        currentMapType
    );

    return true;
}

void FarmScene::onSeasonChanged(const std::string& newSeason) {
    CCLOG("Season changing to: %s", newSeason.c_str());

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

    CCLOG("Season change completed: %s", newSeason.c_str());
}

bool FarmScene::initPlayer() {
    player = User::create("Player", "male", 100, 100, 1000);
    if (!player) {
        return false;
    }

    Size mapSize = tmxMap->getContentSize();
    Vec2 centerPosition = Vec2(mapSize.width / 2, mapSize.height / 2);

    player->setAnchorPoint(Vec2(0.5, 0));
    player->setPosition(centerPosition);
    player->initPhysics();

    this->addChild(player, 999);

    auto camera = this->getDefaultCamera();
    camera->setPosition(centerPosition);

    CCLOG("Player initialized at position: (%.2f, %.2f)", centerPosition.x, centerPosition.y);

    return true;
}

void FarmScene::update(float dt) {
    if (!player || !tmxMap) {
        return;
    }

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

void FarmScene::initInventory() {
    ToolItem* reap = new ToolItem("Reap", "A useful tool.", "tool/reap.png");
    player->getInventory()->addItem(reap);
    ToolItem* chop = new ToolItem("Chop", "A useful tool.", "tool/chop.png");
    player->getInventory()->addItem(chop);
    ToolItem* hoe = new ToolItem("Hoe", "A useful tool.", "tool/hoe.png");
    player->getInventory()->addItem(hoe);
    ToolItem* pickaxe = new ToolItem("Pickaxe", "A useful tool.", "tool/pickaxe.png");
    player->getInventory()->addItem(pickaxe);

    player->getInventory()->reduceItemQuantity(0, 0, 2);
}

// ... [保持原有的键盘和鼠标事件处理代码不变] ...

//输入设备监听

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


