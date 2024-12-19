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
    
    currentBubble = nullptr;
    isShowingBubble = false;
    currentInteraction = InteractionType::NONE;
    
    // 初始化规则
    InteractionRule fishingRule;
    fishingRule.message = "按 F 钓鱼";
    fishingRule.iconPath = "icons/fishing.png";
    fishingRule.checkRadius = 64.0f;
    interactionRules[InteractionType::FISHING] = fishingRule;
    
    InteractionRule farmingRule;
    farmingRule.message = "按 F 耕地";
    farmingRule.iconPath = "icons/farming.png";
    farmingRule.checkRadius = 64.0f;
    interactionRules[InteractionType::FARMING] = farmingRule;
    
    return true;
}

void FarmInteractionManager::update(float dt) {
    if (!player || !mapManager) return;
    checkPlayerPosition();
}

void FarmInteractionManager::checkPlayerPosition() {
    Vec2 playerPos = player->getPosition();
    
    // 检查钓鱼点
    if (mapManager->isFishingSpot(playerPos)) {
        showInteractionBubble(InteractionType::FISHING, 
            playerPos + bubbleOffset);
    }
    // 检查可耕种地点
    else if (mapManager->isArable(playerPos)) {
        showInteractionBubble(InteractionType::FARMING, 
            playerPos + bubbleOffset);
    }
    else {
        hideInteractionBubble();
    }
}

void FarmInteractionManager::showInteractionBubble(InteractionType type, const Vec2& position) {
    if (currentInteraction == type && isShowingBubble) return;
    
    const auto& rule = interactionRules[type];
    createBubble(rule.message, rule.iconPath);
    
    if (currentBubble) {
        currentBubble->setPosition(position);
        currentInteraction = type;
        isShowingBubble = true;
        currentBubble->show();
    }
}

void FarmInteractionManager::hideInteractionBubble() {
    if (currentBubble && isShowingBubble) {
        currentBubble->hide();
        isShowingBubble = false;
        currentInteraction = InteractionType::NONE;
    }
}

void FarmInteractionManager::createBubble(const std::string& message, const std::string& iconPath) {
    if (!currentBubble) {
        currentBubble = InteractionBubble::create(message, iconPath);
        if (currentBubble) {
            currentScene->addChild(currentBubble, 1000);
        }
    } else {
        currentBubble->updateMessage(message);
    }
}

void FarmInteractionManager::cleanup() {
    if (currentBubble) {
        currentBubble->removeFromParent();
        currentBubble = nullptr;
    }
    isShowingBubble = false;
    currentInteraction = InteractionType::NONE;
}

// ... 其他方法的实现 ...
