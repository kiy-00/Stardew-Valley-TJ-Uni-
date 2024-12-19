#pragma once
#include "cocos2d.h"

class InteractionBubble : public cocos2d::Node {
public:
    static InteractionBubble* create(const std::string& message, const std::string& iconPath);
    bool init(const std::string& message, const std::string& iconPath);
    void show();
    void hide();
    void updateMessage(const std::string& message);
    
private:
    cocos2d::Sprite* icon;
    cocos2d::Label* messageLabel;
};
