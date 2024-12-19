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
    
    // 初始化指示精灵并设置在屏幕中间
    fishingSprite = Sprite::create("icons/fishing.png");
    farmingSprite = Sprite::create("icons/farming.png");
    
    if(fishingSprite) {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        fishingSprite->setPosition(Vec2(origin.x + visibleSize.width/2,
                                      origin.y + visibleSize.height/2));
        currentScene->addChild(fishingSprite, 1000);
    }
    
    if(farmingSprite) {
        Size visibleSize = Director::getInstance()->getVisibleSize();
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        farmingSprite->setPosition(Vec2(origin.x + visibleSize.width/2 + 50,  // 稍微偏右一点
                                      origin.y + visibleSize.height/2));
        currentScene->addChild(farmingSprite, 1000);
    }
    
    // 初始化调试标签
    worldPosLabel = Label::createWithTTF("World Pos: ", "fonts/Marker Felt.ttf", 24);
    tilePosLabel = Label::createWithTTF("Tile Pos: ", "fonts/Marker Felt.ttf", 24);
    playerPosLabel = Label::createWithTTF("Player Pos: ", "fonts/Marker Felt.ttf", 24);
    
    if(worldPosLabel) {
        worldPosLabel->setPosition(Vec2(150, 120));
        currentScene->addChild(worldPosLabel, 1000);
    }
    
    if(tilePosLabel) {
        tilePosLabel->setPosition(Vec2(150, 80));
        currentScene->addChild(tilePosLabel, 1000);
    }
    
    if(playerPosLabel) {
        playerPosLabel->setPosition(Vec2(150, 40));
        currentScene->addChild(playerPosLabel, 1000);
    }
    
    return true;
}

void FarmInteractionManager::update(float dt) {
    if (!player || !mapManager) return;
    checkPlayerPosition();
}

void FarmInteractionManager::checkPlayerPosition() {
    Vec2 playerPos = player->getPosition();
    Vec2 tileCoord = mapManager->worldToTileCoord(playerPos);
    
    // 更新调试信息显示
    if(worldPosLabel) {
        std::string worldText = StringUtils::format("World Pos: (%.1f, %.1f)", 
                                                  playerPos.x, playerPos.y);
        worldPosLabel->setString(worldText);
    }
    
    if(tilePosLabel) {
        std::string tileText = StringUtils::format("Tile Pos: (%.1f, %.1f)", 
                                                 tileCoord.x, tileCoord.y);
        tilePosLabel->setString(tileText);
    }
    
    if(playerPosLabel) {
        std::string playerText = StringUtils::format("Player Pos: (%.1f, %.1f)", 
                                                   player->getPositionX(), 
                                                   player->getPositionY());
        playerPosLabel->setString(playerText);
    }
    
    // 检查钓鱼点，只更新位置
    if (mapManager->isFishingSpot(playerPos) && fishingSprite) {
        fishingSprite->setPosition(playerPos + Vec2(0, 50));
    }
    
    // 检查可耕种地点，只更新位置
    if (mapManager->isArable(playerPos) && farmingSprite) {
        farmingSprite->setPosition(playerPos + Vec2(0, 50));
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
    
    if(worldPosLabel) {
        worldPosLabel->removeFromParent();
        worldPosLabel = nullptr;
    }
    if(tilePosLabel) {
        tilePosLabel->removeFromParent();
        tilePosLabel = nullptr;
    }
    if(playerPosLabel) {
        playerPosLabel->removeFromParent();
        playerPosLabel = nullptr;
    }
}


