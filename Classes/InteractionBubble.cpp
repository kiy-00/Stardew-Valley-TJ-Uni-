#include "InteractionBubble.h"

InteractionBubble* InteractionBubble::create(const std::string& message, const std::string& iconPath) {
    auto bubble = new InteractionBubble();
    if (bubble && bubble->init(message, iconPath)) {
        bubble->autorelease();
        return bubble;
    }
    CC_SAFE_DELETE(bubble);
    return nullptr;
}

bool InteractionBubble::init(const std::string& message, const std::string& iconPath) {
    if (!Node::init()) {
        return false;
    }
    
    // 创建图标
    icon = cocos2d::Sprite::create(iconPath);
    if (icon) {
        icon->setPosition(cocos2d::Vec2(-30, 0));
        this->addChild(icon);
    }
    
    // 创建文本
    messageLabel = cocos2d::Label::createWithTTF(message, "fonts/Marker Felt.ttf", 16);
    if (messageLabel) {
        messageLabel->setAnchorPoint(cocos2d::Vec2(0, 0.5f));  // 设置左对齐
        messageLabel->setPosition(cocos2d::Vec2(0, 0));        // 从图标右侧开始显示文本
        this->addChild(messageLabel);
    }
    
    return true;
}

void InteractionBubble::show() {
    this->setVisible(true);
}

void InteractionBubble::hide() {
    this->setVisible(false);
}

void InteractionBubble::updateMessage(const std::string& message) {
    if (messageLabel) {
        messageLabel->setString(message);
    }
}
