#include "FarmInteractionManager.h"

FarmInteractionManager* FarmInteractionManager::instance = nullptr;

FarmInteractionManager* FarmInteractionManager::getInstance() {
    if (!instance) {
        instance = new FarmInteractionManager();
    }
    return instance;
}

bool FarmInteractionManager::init(Scene* scene, User* player, FarmMapManager* mapManager) {
    this->currentScene = scene;
    this->player = player;
    this->mapManager = mapManager;
    
    // 初始化时间系统并启动
    auto timeSystem = TimeSeasonSystem::getInstance();
    if (timeSystem) {
        // 确保 timeSystem 在场景中被 scheduleUpdate
        currentScene->addChild(timeSystem, 9999); // 加到场景
        timeSystem->startTime();
    }
    
    // 初始化指示精灵并设置在屏幕中间
    fishingSprite = Sprite::create("icons/fishing.png");
    fishingSprite->setVisible(false);
    currentScene->addChild(fishingSprite, 1000);
    farmingSprite = Sprite::create("icons/farming.png");
    farmingSprite->setVisible(false);
    currentScene->addChild(farmingSprite, 1000);
    farmPermitSprite = Sprite::create("icons/chicken.png");
    farmPermitSprite->setVisible(false);
    currentScene->addChild(farmPermitSprite, 1000);
    
    // 初始化调试标签
    worldPosLabel = Label::createWithTTF("World Pos: ", "fonts/Marker Felt.ttf", 24);
    tilePosLabel = Label::createWithTTF("Tile Pos: ", "fonts/Marker Felt.ttf", 24);
    timeLabel = Label::createWithTTF("Time: ", "fonts/Marker Felt.ttf", 24);
    
    // 统一设置标签位置
    if(worldPosLabel) {
        worldPosLabel->setPosition(Vec2(150, 80));
        currentScene->addChild(worldPosLabel, 1000);
    }
    
    if(tilePosLabel) {
        tilePosLabel->setPosition(Vec2(150, 40));
        currentScene->addChild(tilePosLabel, 1000);
    }
    
    if(timeLabel) {
        timeLabel->setPosition(Vec2(150, 120));
        currentScene->addChild(timeLabel, 1000);
    }
    
    return true;
}

void FarmInteractionManager::update(float dt) {
    if (!player || !mapManager) return;
    
    // 更新玩家位置和时间信息
    Vec2 playerPos = player->getPosition();
    Vec2 tileCoord = mapManager->worldToTileCoord(playerPos);
    
    // 更新位置标签
    if(worldPosLabel) {
        worldPosLabel->setString(StringUtils::format("World Pos: (%.1f, %.1f)", 
            playerPos.x, playerPos.y));
    }
    
    if(tilePosLabel) {
        tilePosLabel->setString(StringUtils::format("Tile Pos: (%.1f, %.1f)", 
            tileCoord.x, tileCoord.y));
    }
    
    // 更新时间标签
    if (timeLabel) {
        auto tss = TimeSeasonSystem::getInstance();
        if (tss) {
            auto ct = tss->getCurrentTime();
            std::string season;
            switch(ct.season) {
                case 0: season = "Spring"; break;
                case 1: season = "Summer"; break;
                case 2: season = "Autumn"; break;
                case 3: season = "Winter"; break;
                default: season = "Spring";
            }
            timeLabel->setString(StringUtils::format(
                "Year %d %s Day %d %02d:%02d",
                ct.year, season.c_str(), ct.day, ct.hour, ct.minute));
        }
    }

    checkPlayerPosition();
}

void FarmInteractionManager::checkPlayerPosition() {
    Vec2 playerPos = player->getPosition();
    
    // 检查钓鱼点，显示或隐藏提示图标
    if (mapManager->isFishingSpot(playerPos)) {
        fishingSprite->setVisible(true);
        fishingSprite->setPosition(playerPos + Vec2(0, 50));
    } else {
        fishingSprite->setVisible(false);
    }

    // 更新可耕种地点提示图标
    if (mapManager->isArable(playerPos)) {
        farmingSprite->setVisible(true);
        farmingSprite->setPosition(playerPos + Vec2(0, 50));
    } else {
        farmingSprite->setVisible(false);
    }

    // 更新可养殖点提示图标
    if (mapManager->isFarmPermit(playerPos)) {
        farmPermitSprite->setVisible(true);
        farmPermitSprite->setPosition(playerPos + Vec2(0, 50));
    } else {
        farmPermitSprite->setVisible(false);
    }
}

// 注释掉原来的气泡相关代码
/*
void FarmInteractionManager::showInteractionBubble(...) {
    // ...原有代码...
}

void FarmInteractionManager::hideInteractionBubble() {
    // ...原有代码...
}

void FarmInteractionManager::createBubble(...) {
    // ...原有代码...
}
*/

void FarmInteractionManager::cleanup() {
    if(fishingSprite) {
        fishingSprite->removeFromParent();
        fishingSprite = nullptr;
    }
    if(farmingSprite) {
        farmingSprite->removeFromParent();
        farmingSprite = nullptr;
    }

    if (farmPermitSprite) {
        farmPermitSprite->removeFromParent();
        farmPermitSprite = nullptr;
    }
    
    if(worldPosLabel) {
        worldPosLabel->removeFromParent();
        worldPosLabel = nullptr;
    }
    if(tilePosLabel) {
        tilePosLabel->removeFromParent();
        tilePosLabel = nullptr;
    }
    if(timeLabel) {
        timeLabel->removeFromParent();
        timeLabel = nullptr;
    }
}


