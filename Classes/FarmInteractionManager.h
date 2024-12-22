#ifndef FARM_INTERACTION_MANAGER_H
#define FARM_INTERACTION_MANAGER_H

#include "cocos2d.h"
#include "User.h"
#include "InteractionBubble.h"
#include "FarmMapManager.h"
#include "TimeSeasonSystem.h"  // 添加这一行

USING_NS_CC;

// 交互类型枚举
enum class InteractionType {
    NONE,
    FISHING,
    FARMING
};

// 简化的交互规则结构体
struct InteractionRule {
    std::string message;        // 提示信息
    std::string iconPath;       // 图标路径
    float checkRadius;          // 检测半径
};

class FarmInteractionManager {
public:
    static FarmInteractionManager* getInstance();
    
    bool init(Scene* scene, User* player, FarmMapManager* mapManager);
    void update(float dt);
    void showInteractionBubble(InteractionType type, const Vec2& position);
    void hideInteractionBubble();
    void cleanup();
    
    // 添加系统事件响应接口
    void onSeasonChanged(const std::string& season) {}  // 暂时为空实现
    void onWeatherChanged(const std::string& weather) {} // 暂时为空实现
    void onTimeChanged(float gameTime) {} // 暂时为空实现

private:
    FarmInteractionManager() {}
    void checkPlayerPosition();
    void createBubble(const std::string& message, const std::string& iconPath);
    void removeBubble();

private:
    static FarmInteractionManager* instance;
    Scene* currentScene;
    User* player;
    FarmMapManager* mapManager;
    InteractionBubble* currentBubble;
    std::map<InteractionType, InteractionRule> interactionRules;
    InteractionType currentInteraction;
    bool isShowingBubble;
    Vec2 bubbleOffset = Vec2(0, 50);
    Sprite* fishingSprite;
    Sprite* farmingSprite;
    Sprite* farmPermitSprite;

    Label* worldPosLabel;
    Label* tilePosLabel;
    cocos2d::Label* timeLabel = nullptr;
};

#endif
