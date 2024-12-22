// FarmScene.cpp
#include "FarmScene.h"
#include "FarmMapManager.h"
#include "FarmMapRenderer.h"
#include "SpritePathManager.h"
#include "TimeSeasonSystem.h"
#include "RenderConstants.h"
#include "WeatherSystem.h"
#include "WeatherEffectManager.h"
#include "StandardItem.h"
#include "BreedItem.h"
#include "AudioManager.h"


USING_NS_CC;

Scene* FarmScene::createScene(const std::string& mapType) {
    return FarmScene::create(mapType);
}

bool FarmScene::init(const std::string& mapType) {
    if (!Scene::initWithPhysics()) {
        return false;
    }

    // 初始化物理世界
    auto physicsWorld = this->getPhysicsWorld();
    physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    physicsWorld->setGravity(Vec2::ZERO);

    currentMapType = mapType;

    // 按顺序初始化各个系统
    if (!initTimeSystem() || !initWeatherSystem() || !createSystemLabels()) {
        CCLOG("Failed to initialize core systems");
        return false;
    }

    // 初始化地图管理器
    farmMapManager = FarmMapManager::getInstance();
    if (!farmMapManager->initWithFarmType(mapType, timeSystem->getCurrentSeasonString())) {
        return false;
    }

    // 初始化其他游戏组件
    if (!initMap() || !initPlayer()) {
        return false;
    }

    // 初始化剩余组件
    setupKeyboard();
    setupMouse();
    initInventory();
    initFarmland();

    // 初始化交互管理器
    interactionManager = FarmInteractionManager::getInstance();
    interactionManager->init(this, player, farmMapManager);

   

    // 使用新的音频管理器 - 延迟初始化以确保其他系统已经就绪
   /* this->scheduleOnce([this](float dt) {
        auto audioManager = AudioManager::getInstance();
        if (audioManager && audioManager->init()) {
            audioManager->setBackgroundMusicVolume(0.8f);
            audioManager->playBackgroundMusic("music/bgmusic.mp3", true);
            audioManager->logAudioStatus();
        }
        }, 0.5f, "init_audio");*/

    this->schedule([this](float dt) {
        interactionManager->update(dt);
        }, 0.1f, "interaction_update");

    this->scheduleUpdate();

    // 创建物品栏
    this->scheduleOnce([this](float dt) {
        player->createInventoryBar();
        }, 0.1f, "createInventoryBarKey");

    // 启动所有系统
    startSystems();

    // 设置回调和调度器
    setupSystemCallbacks();
    setupSystemSchedulers();

    weatherEffectManager->updateWeatherEffect(weatherSystem->getCurrentWeatherString());

    return true;
}

bool FarmScene::initTimeSystem() {
    timeSystem = TimeSeasonSystem::getInstance();
    if (!timeSystem) {
        CCLOG("Failed to create TimeSeasonSystem");
        return false;
    }
    timeSystem->setName("TimeSeasonSystem");
    timeSystem->retain();
    this->addChild(timeSystem);

    // Initialize time effect manager
    timeEffectManager = TimeEffectManager::getInstance();
    timeEffectManager->setName("TimeEffectManager");
    timeEffectManager->retain();
    this->addChild(timeEffectManager, FIFTH);
    timeEffectManager->initializeWithScene(this);
    return true;
}

bool FarmScene::initWeatherSystem() {
    weatherSystem = WeatherSystem::getInstance();
    if (!weatherSystem) {
        CCLOG("Failed to create WeatherSystem");
        return false;
    }
    weatherSystem->setName("WeatherSystem");
    weatherSystem->retain();
    this->addChild(weatherSystem);

    // 初始化天气特效管理器
    weatherEffectManager = WeatherEffectManager::getInstance();
    if (!weatherEffectManager) {
        CCLOG("Failed to create WeatherEffectManager");
        return false;
    }
    weatherEffectManager->setName("WeatherEffectSystem");
    weatherEffectManager->retain();
    this->addChild(weatherEffectManager, FIFTH);
    weatherEffectManager->initializeWithScene(this);

    return true;
}

void FarmScene::setupSystemCallbacks() {
    // 设置季节变化回调
    timeSystem->setSeasonChangedCallback([this](const std::string& newSeason) {
        this->onSeasonChanged(newSeason);
        });

    // 设置天气变化回调
    weatherSystem->setWeatherChangedCallback([this](const std::string& newWeather) {
        this->onWeatherChanged(newWeather);
        });

    // 添加时间变化回调
    timeSystem->setTimeChangedCallback([this](const TimeSeasonSystem::GameTime& newTime) {
        if (timeEffectManager) {
            timeEffectManager->updateTimeEffect(newTime.hour);
            CCLOG("Time effect updated for hour: %d", newTime.hour);
        }
        });

}

bool FarmScene::createSystemLabels() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建天气标签
    weatherLabel = Label::createWithTTF("Weather: Sunny", "fonts/Marker Felt.ttf", 24);
    if (!weatherLabel) {
        CCLOG("Failed to create weatherLabel");
        return false;
    }
    weatherLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 60));
    this->addChild(weatherLabel, 1000);

    return true;
}

void FarmScene::setupSystemSchedulers() {
   
    // 设置天气标签更新调度器
    this->schedule([this](float dt) {
        this->updateWeatherLabel();
        }, 0.1f, "weather_update");
}

void FarmScene::startSystems() {
    // 启动时间系统
    if (timeSystem) {
        timeSystem->startTime();
        CCLOG("Time system started successfully");
    }

    // 启动天气系统
    if (weatherSystem) {
        weatherSystem->startWeatherSystem();
        CCLOG("Weather system started successfully");
        // 在场景的 init 函数中
    }
  
    updateWeatherLabel();
}


bool FarmScene::initMap() {
    tmxMap = farmMapManager->getMap();
    if (!tmxMap) {
        //CCLOG("Failed to get map from FarmMapManager");
        return false;
    }
    this->addChild(tmxMap, FIRST);  // 修改这里，使用 FIRST
    FarmMapRenderer::getInstance()->renderMap(
        tmxMap,
        timeSystem->getCurrentSeasonString(),
        currentMapType
    );

    return true;
}

void FarmScene::onSeasonChanged(const std::string& newSeason) {
    //CCLOG("Season changing to: %s", newSeason.c_str());

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

    //CCLOG("Season change completed: %s", newSeason.c_str());
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

    this->addChild(player, THIRD);  // 修改这里，使用 THIRD

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

    /* --- 添加背包位置更新逻辑（相对人物显示） --- */
    if (player->getIsInventoryOpen()) {
        auto inventoryLayer = Director::getInstance()->getRunningScene()->getChildByName("InventoryLayer");
        if (inventoryLayer) {
            Vec2 inventoryOffset = Vec2(-125, 100);
            inventoryLayer->setPosition(playerPos + inventoryOffset);
        }
    }
}

// Add new methods to FarmScene.cpp:
void FarmScene::updateWeatherLabel() {
    if (!weatherLabel || !weatherSystem) return;

    std::string weatherStr = weatherSystem->getCurrentWeatherString();
    CCLOG("Current weather: %s", weatherStr.c_str());  // 正确的CCLOG用法
    weatherLabel->setString("Weather: " + weatherStr);
}

void FarmScene::onWeatherChanged(const std::string& newWeather) {
    // Here you can handle weather change effects
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
	RodItem* rod = new RodItem("Rod", "A useful tool.", "tool/rod.png");
	player->getInventory()->addItem(rod);
	ToolItem* kettle = new ToolItem("Kettle", "A useful tool.", "tool/kettle.png");
	player->getInventory()->addItem(kettle);

    //默认玩家一开始就有很多种子
    BerrySeed* berrySeeds = BerrySeed::create();
    player->getInventory()->addItem(berrySeeds);

    CarrotSeed* carrotSeeds = CarrotSeed::create();
    player->getInventory()->addItem(carrotSeeds);

    PepperSeed* pepperSeeds = PepperSeed::create();
    player->getInventory()->addItem(pepperSeeds);

    WheatSeed* wheatSeeds = WheatSeed::create();
    player->getInventory()->addItem(wheatSeeds);
    
}


void FarmScene::initFarmland() {
    CCLOG("=== Begin Tile Size Diagnostics ===");

    // 获取基本信息
    Size tileSize = tmxMap->getTileSize();
    Size mapSize = tmxMap->getMapSize();

    // 检查地图缩放
    float mapScaleX = tmxMap->getScaleX();
    float mapScaleY = tmxMap->getScaleY();
    CCLOG("Map scale: X=%.2f, Y=%.2f", mapScaleX, mapScaleY);

    // 计算实际瓦片大小（考虑缩放）
    float actualTileWidth = tileSize.width * mapScaleX;
    float actualTileHeight = tileSize.height * mapScaleY;
    CCLOG("Base tile size: %.2f x %.2f", tileSize.width, tileSize.height);
    CCLOG("Scaled tile size: %.2f x %.2f", actualTileWidth, actualTileHeight);

    // 获取一个示例瓦片的信息
    auto layer = tmxMap->getLayer("arable");
    if (layer) {
        Vec2 testPos(25, 15);
        auto tileSprite = layer->getTileAt(testPos);
        if (tileSprite) {
            Size tileSpriteSize = tileSprite->getContentSize();
            float tileScaleX = tileSprite->getScaleX();
            float tileScaleY = tileSprite->getScaleY();
            CCLOG("Individual tile scale: X=%.2f, Y=%.2f", tileScaleX, tileScaleY);
            CCLOG("Individual tile content size: %.2f x %.2f", tileSpriteSize.width, tileSpriteSize.height);
            CCLOG("Individual tile actual size: %.2f x %.2f",
                tileSpriteSize.width * tileScaleX,
                tileSpriteSize.height * tileScaleY);
        }
    }

    // 创建耕地时也应用相同的缩放
    auto farmablePositions = getFarmablePositions();
    farmlandManager = FarmlandManager::getInstance();
    FarmlandTile::setTileSize(actualTileWidth);  // 使用实际大小
    farmlandManager->init(this, farmablePositions, SECOND);

  
    CCLOG("=== End Diagnostics ===");


    //创建作物管理者。
    cropManager = CropManager::getInstance();

    cropManager->init(this, THIRD);
}


std::vector<Vec2> FarmScene::getFarmablePositions() {
    std::vector<Vec2> positions;
    Size mapSize = tmxMap->getContentSize();
    Size tileSize = tmxMap->getTileSize();

    // 遍历地图上的每个位置
    for (float x = 0; x < mapSize.width; x += tileSize.width) {
        for (float y = 0; y < mapSize.height; y += tileSize.height) {
            Vec2 pos(x, y);
            //这里是世界坐标
            auto renderPos = Vec2(x, y);
            if (farmMapManager->isArable(pos)) {
                positions.push_back(renderPos);
                CCLOG("Found arable position at: (%.2f, %.2f)", pos.x, pos.y);
            }
        }
    }
    return positions;
}



void FarmScene::setupMouse() {
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseDown = CC_CALLBACK_1(FarmScene::onMouseClick, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
}

void FarmScene::onMouseClick(EventMouse* event) {


    // 获取原始鼠标位置
    Vec2 mousePosition = event->getLocation();

   if(event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
            /* -----新增：执行行为时传入地图管理者---- */
    player->performAction(farmMapManager);

         
}


//void FarmScene::setupKeyboard() {
//    auto keyboardListener = EventListenerKeyboard::create();
//
//    // 添加按键按下事件
//    keyboardListener->onKeyPressed = CC_CALLBACK_2(FarmScene::onKeyPressed, this);
//
//    // 添加按键释放事件
//    keyboardListener->onKeyReleased = CC_CALLBACK_2(FarmScene::onKeyReleased, this);
//
//    // 使用addEventListenerWithFixedPriority替代
//    _eventDispatcher->addEventListenerWithFixedPriority(keyboardListener, 1);
//
//    //CCLOG("键盘事件监听器设置完成");
//}

//void FarmScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
//    switch (keyCode) {
//        case EventKeyboard::KeyCode::KEY_UP_ARROW:
//        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
//        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
//        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
//        case EventKeyboard::KeyCode::KEY_W:
//        case EventKeyboard::KeyCode::KEY_A:
//        case EventKeyboard::KeyCode::KEY_S:
//        case EventKeyboard::KeyCode::KEY_D:
//            player->setMoving(false);
//            break;
//        default:
//            break;
//    }
//}


//void FarmScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
//    float moveDistance = 16.0f;
//    Vec2 currentPos = player->getPosition();
//    Vec2 targetPos = currentPos;
//    int index = -1;
//
//    /* ------------------养殖系统-------------- */
//    // 创建动物
//    switch (keyCode) {
//	case EventKeyboard::KeyCode::KEY_C: {
//		if (farmMapManager->isFarmPermit(targetPos)) {
//			createAnimal("chicken", targetPos);
//		}
//		break;
//	}
//	case EventKeyboard::KeyCode::KEY_V: {
//		if (farmMapManager->isFarmPermit(targetPos)) {
//			createAnimal("sheep", targetPos);
//		}
//		break;
//	}
//	case EventKeyboard::KeyCode::KEY_M: {
//		interactWithAnimals(targetPos, 50.0f);
//		break;
//	}
//	case EventKeyboard::KeyCode::KEY_G: {
//		pickupNearbyItems(targetPos, 50.0f);
//		break;
//	}
//    }
//
//    /* ----------------物品拾取---------------- */
//	if (keyCode == EventKeyboard::KeyCode::KEY_G) {
//        // 拾取范围半径
//		float pickupRadius = 50.0f; 
//
//		// 获取场景中的所有物品
//		std::vector<Sprite*> items;
//		for (auto child : this->getChildren()) {
//			auto sprite = static_cast<Sprite*>(child);
//			if (sprite && static_cast<Item*>(sprite->getUserData())) { // 检查是否绑定了 Item
//				items.push_back(sprite);
//			}
//		}
//		// 调用玩家拾取方法
//		player->pickupNearbyItems(items, pickupRadius);
//	}
//
//    // 数字键选择物品
//    switch (keyCode) {
//        case EventKeyboard::KeyCode::KEY_1:
//        case EventKeyboard::KeyCode::KEY_2:
//        case EventKeyboard::KeyCode::KEY_3:
//        case EventKeyboard::KeyCode::KEY_4:
//        case EventKeyboard::KeyCode::KEY_5:
//        case EventKeyboard::KeyCode::KEY_6:
//        case EventKeyboard::KeyCode::KEY_7:
//        case EventKeyboard::KeyCode::KEY_8:
//            index = static_cast<int>(keyCode) - static_cast<int>(EventKeyboard::KeyCode::KEY_1);
//            player->setSelectedSlot(0, index);
//            player->selectItemFromInventory(index);
//            break;
//
//        // 移动控制
//        case EventKeyboard::KeyCode::KEY_W:
//        case EventKeyboard::KeyCode::KEY_UP_ARROW:
//            targetPos.y += moveDistance;
//            player->moveUp();
//            break;
//        case EventKeyboard::KeyCode::KEY_S:
//        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
//            targetPos.y -= moveDistance;
//            player->moveDown();
//            break;
//        case EventKeyboard::KeyCode::KEY_A:
//        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
//            targetPos.x -= moveDistance;
//            player->moveLeft();
//            break;
//        case EventKeyboard::KeyCode::KEY_D:
//        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
//            targetPos.x += moveDistance;
//            player->moveRight();
//            break;
//
//        // 背包和物品控制
//        case EventKeyboard::KeyCode::KEY_E:
//            if (!player->getIsSlotImageOpen()) {
//                player->toggleInventory();
//            }
//            break;
//        case EventKeyboard::KeyCode::KEY_R:
//            player->reduceSelectedItemQuantity(1, true);
//            break;
//        case EventKeyboard::KeyCode::KEY_N:
//            if (!player->getIsInventoryOpen()) {
//                player->toggleSlotImage();
//            }
//            break;
//        default:
//            return;
//    }
//
//    // 处理移动逻辑
//    if (keyCode == EventKeyboard::KeyCode::KEY_W ||
//        keyCode == EventKeyboard::KeyCode::KEY_S ||
//        keyCode == EventKeyboard::KeyCode::KEY_A ||
//        keyCode == EventKeyboard::KeyCode::KEY_D ||
//        keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW ||
//        keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW ||
//        keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
//        keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
//
//        bool canWalk = farmMapManager->isWalkable(targetPos);
//        if (canWalk) {
//            auto moveAction = MoveTo::create(0.2f, targetPos);
//            auto callback = CallFunc::create([this, targetPos]() {
//                bool isPenetrable = farmMapManager->isPenetrable(targetPos);
//                player->updateVisibility(isPenetrable);
//                player->stopMoving();
//                });
//            auto sequence = Sequence::create(moveAction, callback, nullptr);
//            player->runAction(sequence);
//        }
//    }
//}

void FarmScene::setupKeyboard() {
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = CC_CALLBACK_2(FarmScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = CC_CALLBACK_2(FarmScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithFixedPriority(keyboardListener, 1);
}

void FarmScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
    // 如果背包打开，不处理移动释放事件
    if (player->getIsInventoryOpen()) {
        return;
    }

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

//void FarmScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
//    float moveDistance = 16.0f;
//    Vec2 currentPos = player->getPosition();
//    Vec2 targetPos = currentPos;
//
//    // 在 FarmScene::onKeyPressed 中的背包导航部分
//    if (player->getIsInventoryOpen()) {
//        auto currentSlot = player->getSelectedSlot();
//        int row = currentSlot.first;
//        int col = currentSlot.second;
//
//        int newRow = 0;
//
//
//
//        switch (keyCode) {
//        case EventKeyboard::KeyCode::KEY_UP_ARROW:
//        case EventKeyboard::KeyCode::KEY_W:
//            row = (row + 1) % 3;  // 3行，从0到2
//            player->setSelectedSlot(row, col);
//            player->highlightInventorySlot(row, col);
//            return;
//
//        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
//        case EventKeyboard::KeyCode::KEY_S:
//         
//            row = (row - 1 + 3) % 3;  // 3行，从0到2
//            player->setSelectedSlot(row, col);
//            player->highlightInventorySlot(row, col);
//            return;
//
//        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
//        case EventKeyboard::KeyCode::KEY_A:
//            col = (col - 1 + 8) % 8;  // 8列，从0到7
//            player->setSelectedSlot(row, col);
//            player->highlightInventorySlot(row, col);
//            return;
//
//        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
//        case EventKeyboard::KeyCode::KEY_D:
//            col = (col + 1) % 8;  // 8列，从0到7
//            player->setSelectedSlot(row, col);
//            player->highlightInventorySlot(row, col);
//            return;
//
//        case EventKeyboard::KeyCode::KEY_SPACE:
//            // 直接使用当前行列来选择物品
//            player->setSelectedSlot(row, col);
//            player->selectItemFromInventory(row, col);  // 修改这个函数接收行和列
//            return;
//
//        case EventKeyboard::KeyCode::KEY_E:
//            player->toggleInventory();
//            return;
//
//        default:
//            return;
//        }
//    }
//    // 背包关闭时的常规按键处理
//    switch (keyCode) {
//        // 养殖系统相关
//    case EventKeyboard::KeyCode::KEY_C:
//        if (farmMapManager->isFarmPermit(targetPos)) {
//            createAnimal("chicken", targetPos);
//        }
//        break;
//    case EventKeyboard::KeyCode::KEY_V:
//        if (farmMapManager->isFarmPermit(targetPos)) {
//            createAnimal("sheep", targetPos);
//        }
//        break;
//    case EventKeyboard::KeyCode::KEY_M:
//        interactWithAnimals(targetPos, 50.0f);
//        break;
//    case EventKeyboard::KeyCode::KEY_G:
//        pickupNearbyItems(targetPos, 50.0f);
//        break;
//
//        // 移动控制
//    case EventKeyboard::KeyCode::KEY_W:
//    case EventKeyboard::KeyCode::KEY_UP_ARROW:
//        targetPos.y += moveDistance;
//        player->moveUp();
//        break;
//    case EventKeyboard::KeyCode::KEY_S:
//    case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
//        targetPos.y -= moveDistance;
//        player->moveDown();
//        break;
//    case EventKeyboard::KeyCode::KEY_A:
//    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
//        targetPos.x -= moveDistance;
//        player->moveLeft();
//        break;
//    case EventKeyboard::KeyCode::KEY_D:
//    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
//        targetPos.x += moveDistance;
//        player->moveRight();
//        break;
//
//        // 在打开背包时设置初始选择
//    case EventKeyboard::KeyCode::KEY_E:
//        if (!player->getIsSlotImageOpen()) {
//            player->toggleInventory();
//            if (player->getIsInventoryOpen()) {
//                player->setSelectedSlot(0, 0);  // 设置初始选择位置
//                player->highlightInventorySlot(0, 0);
//            }
//        }
//        break;
//    case EventKeyboard::KeyCode::KEY_R:
//        player->reduceSelectedItemQuantity(1, true);
//        break;
//    case EventKeyboard::KeyCode::KEY_N:
//        if (!player->getIsInventoryOpen()) {
//            player->toggleSlotImage();
//        }
//        break;
//    default:
//        return;
//    }
//
//    // 处理移动逻辑
//    if (keyCode == EventKeyboard::KeyCode::KEY_W ||
//        keyCode == EventKeyboard::KeyCode::KEY_S ||
//        keyCode == EventKeyboard::KeyCode::KEY_A ||
//        keyCode == EventKeyboard::KeyCode::KEY_D ||
//        keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW ||
//        keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW ||
//        keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW ||
//        keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
//
//        bool canWalk = farmMapManager->isWalkable(targetPos);
//        if (canWalk) {
//            auto moveAction = MoveTo::create(0.2f, targetPos);
//            auto callback = CallFunc::create([this, targetPos]() {
//                bool isPenetrable = farmMapManager->isPenetrable(targetPos);
//                player->updateVisibility(isPenetrable);
//                player->stopMoving();
//                });
//            auto sequence = Sequence::create(moveAction, callback, nullptr);
//            player->runAction(sequence);
//        }
//    }
//}

void FarmScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
    float moveDistance = 16.0f;
    Vec2 currentPos = player->getPosition();
    Vec2 targetPos = currentPos;

    // 1) 如果背包已打开，则进行背包导航逻辑
    if (player->getIsInventoryOpen()) {
        auto currentSlot = player->getSelectedSlot();
        int row = currentSlot.first;
        int col = currentSlot.second;

        switch (keyCode) {
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
        case EventKeyboard::KeyCode::KEY_W:
            row = (row + 1) % 3;  // 上键 = row+1
            player->setSelectedSlot(row, col);
            player->highlightInventorySlot(row, col);
            return;
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
        case EventKeyboard::KeyCode::KEY_S:
            row = (row - 1 + 3) % 3;  // 下键 = row-1
            player->setSelectedSlot(row, col);
            player->highlightInventorySlot(row, col);
            return;
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case EventKeyboard::KeyCode::KEY_A:
            col = (col - 1 + 8) % 8;
            player->setSelectedSlot(row, col);
            player->highlightInventorySlot(row, col);
            return;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case EventKeyboard::KeyCode::KEY_D:
            col = (col + 1) % 8;
            player->setSelectedSlot(row, col);
            player->highlightInventorySlot(row, col);
            return;
        case EventKeyboard::KeyCode::KEY_SPACE:
            // 按空格就确认选中
            player->selectItemFromInventory(row, col);
            return;
        case EventKeyboard::KeyCode::KEY_E:
            // 关闭背包
            player->toggleInventory();
            return;

            // ------------------ 新增：L键丢弃物品 ------------------
        case EventKeyboard::KeyCode::KEY_L: {
            // 如果有选中物品，就把它丢到“垃圾桶”
            Item* selectedItem = player->getSelectedItem();
            if (selectedItem) {
                // 这里有两种实现方式：
                // 方式1：直接调用 player->onSlotClicked(0, 8) 模拟“垃圾桶槽位”点击
                player->onSlotClicked(0, 8);

                // 方式2：直接把选中物品数量清零
                //player->reduceSelectedItemQuantity(selectedItem->getQuantity(), false);

                //// 更新背包/横栏显示
                //player->updateInventoryDisplay();
                //createInventoryBarIfNeeded(); // 这里按你自己的代码来，可能是 player->createInventoryBar() 之类
            }
            break;
        }
        default:
            return;
        }
    }

    // 2) 背包关闭时的常规按键处理
    switch (keyCode) {
        // ------------------ 养殖系统 ------------------
    case EventKeyboard::KeyCode::KEY_C:
        if (farmMapManager->isFarmPermit(targetPos)) {
            createAnimal("chicken", targetPos);
        }
        break;
    case EventKeyboard::KeyCode::KEY_V:
        if (farmMapManager->isFarmPermit(targetPos)) {
            createAnimal("sheep", targetPos);
        }
        break;
    case EventKeyboard::KeyCode::KEY_M:
        interactWithAnimals(targetPos, 50.0f);
        break;
    case EventKeyboard::KeyCode::KEY_G:
        // 拾取范围半径 = 50 像素
        pickupNearbyItems(targetPos, 50.0f);
        break;


                                      // ------------------ 新增：F键耕地 / 浇水 ------------------
    case EventKeyboard::KeyCode::KEY_F: {
        // 1. 获取玩家面朝方向
        int direction = player->getDirection();
        // 2. 计算前方 32 像素(1格)的世界坐标 targetPos
        switch (direction) {
        case 0: // 下
            targetPos.y -= 32.0f;
            break;
        case 1: // 上
            targetPos.y += 32.0f;
            break;
        case 2: // 左
            targetPos.x -= 32.0f;
            break;
        case 3: // 右
            targetPos.x += 32.0f;
            break;
        }

        // 3. 获取当前选中的物品
        Item* selectedItem = player->getSelectedItem();
        if (!selectedItem) {
            CCLOG("No tool selected to farm with KEY_F");
            break;
        }

        // 4. 判断选中的物品类型
        std::string itemName = selectedItem->getName(); // 例如 "Hoe" / "Kettle"

        std::string itemType = selectedItem->getItemType();

        // 转换成瓦片坐标
        Vec2 tilePos = farmMapManager->worldToTileCoord(targetPos);

        // 5. 判定是否可耕种
        bool canFarm = farmMapManager->isArable(targetPos);

        bool canPlant = farmlandManager->canPlant(tilePos);

     

        auto tile = farmlandManager->getFarmlandAt(tilePos);
        auto crop = cropManager->getCropOnTile(tile);

        bool canHarvest = false;

        if (crop) {
           canHarvest = crop->canHarvest();
        }

        if (itemName == "Hoe" && canFarm) {
            farmlandManager->handleToolAction(itemName, tilePos, direction);
            CCLOG("Used Hoe at tile (%.0f, %.0f)", tilePos.x, tilePos.y);
        }
        else if (itemName == "Kettle" && canFarm) {
            farmlandManager->handleToolAction(itemName, tilePos, direction);
            CCLOG("Used Kettle at tile (%.0f, %.0f)", tilePos.x, tilePos.y);
        }
        else if (itemName == "Reap" && canHarvest) {
           
            

            auto cropType = crop->getType();

            auto imagePath = "crops/" + cropType + "/" + cropType + "_3.png";


           
            CropItem* cropItem = new CropItem(cropType, "crop, programmer tired, no detailed description", imagePath);
            player->getInventory()->addItem(cropItem);

            cropManager->removeCrop(tile, true);

        }
        else if (itemType == "seed" && canPlant) {

            Crop* newCrop = nullptr;

            if (itemName == "Berry Seed")
            {
                newCrop = CropManager::getInstance()->plantCrop("berry", tile);
            }
            else if (itemName == "Wheat Seed")
            {
                newCrop = CropManager::getInstance()->plantCrop("wheat", tile);
            }
            else if (itemName == "Pepper Seed")
            {
                newCrop = CropManager::getInstance()->plantCrop("pepper", tile);
            }
            else if (itemName == "Carrot Seed")
            {
                newCrop = CropManager::getInstance()->plantCrop("carrot", tile);
            }
          
            if (newCrop) {
                CCLOG("Successfully planted Crop at (%.1f, %.1f)", tile->getPosition().x, tile->getPosition().y);
            }
        }
        else {
            CCLOG("Tool action failed or not a Hoe/Kettle");
        }


        break;
    }

                                      // ------------------ 移动控制 ------------------
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

        // ------------------ 背包开启 ------------------
    case EventKeyboard::KeyCode::KEY_E:
        if (!player->getIsSlotImageOpen()) {
            player->toggleInventory();
            if (player->getIsInventoryOpen()) {
                player->setSelectedSlot(0, 0);  // 设置初始选择位置
                player->highlightInventorySlot(0, 0);
            }
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

    // 3) 处理移动逻辑
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


/* ----------------养殖系统-------------------- */
void FarmScene::createAnimal(const std::string& type, const Vec2& playerPos) {
	Vec2 spawnPos = playerPos - Vec2(30, 0); // 玩家左侧 30 像素
	Animal* animal = AnimalFactory::createAnimal(type);
	if (animal) {
		animal->setPosition(spawnPos);
		animal->produceCallback = [this](const std::string& itemName, const Vec2& position) {
			generateItem(itemName, position);
			};
		this->addChild(animal, SIX);
		CCLOG("%s created at position: (%.2f, %.2f)", type.c_str(), spawnPos.x, spawnPos.y);
	}
}


void FarmScene::interactWithAnimals(const Vec2& playerPos, float interactionRadius) {
	for (auto child : this->getChildren()) {
		auto animal = dynamic_cast<Animal*>(child);
		if (animal && playerPos.distance(animal->getPosition()) <= interactionRadius) {
			animal->touchAnimal();
			CCLOG("Interacted with animal: %s", animal->getAnimalType().c_str());
		}
	}
}

//void FarmScene::pickupNearbyItems(const Vec2& playerPos, float pickupRadius) {
//	for (auto child : this->getChildren()) {
//		auto sprite = dynamic_cast<Sprite*>(child);
//		if (sprite && sprite->getName() == "item" && playerPos.distance(sprite->getPosition()) <= pickupRadius) {
//			auto item = static_cast<Item*>(sprite->getUserData());
//			if (item) {
//				player->getInventory()->addItem(item);
//				sprite->removeFromParent();
//				CCLOG("Picked up item: %s", item->getName().c_str());
//			}
//		}
//	}
//}

void FarmScene::pickupNearbyItems(const Vec2& playerPos, float pickupRadius) {
	std::vector<Node*> itemsToRemove; // 临时容器，存储待移除的节点

	// 遍历场景中的所有子节点
	for (auto child : this->getChildren()) {
		auto sprite = dynamic_cast<Sprite*>(child);
		if (sprite && sprite->getName() == "item" && playerPos.distance(sprite->getPosition()) <= pickupRadius) {
			auto item = static_cast<Item*>(sprite->getUserData());
			if (item) {
				player->getInventory()->addItem(item);
				itemsToRemove.push_back(sprite); // 收集待移除的节点
				CCLOG("Picked up item: %s", item->getName().c_str());
			}
		}
	}

	// 批量移除已收集的节点
	for (auto itemNode : itemsToRemove) {
		itemNode->removeFromParent(); // 移除节点
	}
}

void FarmScene::generateItem(const std::string& itemName, const Vec2& position) {
	std::string itemPath = "animals/" + itemName;
	Sprite* itemSprite = Sprite::create(itemPath);
	if (itemSprite) {
		itemSprite->setPosition(position);
		itemSprite->setName("item");

		// 绑定 Item 对象
		BreedItem* item = new BreedItem(itemName, "Produced by animal", itemPath);
		itemSprite->setUserData(static_cast<void*>(item));

		this->addChild(itemSprite, FIFTH);
		CCLOG("Generated item: %s at (%.2f, %.2f)", itemName.c_str(), position.x, position.y);
	}
}

