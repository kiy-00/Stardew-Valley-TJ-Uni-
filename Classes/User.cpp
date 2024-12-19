#include "User.h"

USING_NS_CC;

User* User::create(const std::string& name, const std::string& gender, int health, int energy,int money) {
    User* pRet = new(std::nothrow) User();
    if (pRet && pRet->init(name, gender, health, energy, money)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}
bool User::init(const std::string& name, const std::string& gender, int health, int energy,int money) {
    // Initialize user properties
    m_name = name;
    m_gender = gender;
    m_health = health;
    m_energy = energy;
    m_money = money;
    m_direction = 0; 
    m_isMoving = false;

    heldItemSprite = cocos2d::Sprite::create(); // 创建新的 Sprite
    this->addChild(heldItemSprite, 1);  // 将其添加到用户的节点下
    heldItemSprite->setVisible(false);

    inventory = new Inventory();

    // 初始化背包状态
    isInventoryOpen = false; // 背包开始时是关闭的

    loadAnimationFrames();

    this->setSpriteFrame(m_animationFrames[0].at(0)); // Default frame (down facing)

    m_frameIndex = 0;  // 初始化帧索引
    m_updateCounter = 0;
    schedule(CC_SCHEDULE_SELECTOR(User::updateAnimation), 0.1f);

    return true;
}

void User::updateAnimation(float dt) {
    m_updateCounter++;
    if (m_updateCounter % 5 == 0) {
        if (m_isMoving) {
            m_frameIndex = (m_frameIndex + 1) % 4;  // 假设每个方向都有4帧
            heldItemSprite->setVisible(false);
            // 根据方向更新精灵帧
            this->setSpriteFrame(m_animationFrames[m_direction].at(m_frameIndex));
        }
        else {
            this->setSpriteFrame(m_animationFrames[m_direction].at(0));//站立
            if (selectedSlot.first != -1 && selectedSlot.second != -1) {
                auto items = inventory->getItems(selectedSlot.first, selectedSlot.second);
                if (!items.empty() && items[0]->getQuantity() > 0) {
                    heldItemSprite->setVisible(true); // 显示物品
                }
                else {
                    heldItemSprite->setVisible(false); // 隐藏物品
                    return;
                }
            }
            else {
                heldItemSprite->setVisible(false); // 若没选中物品，隐藏
                return;
            }
            switch (m_direction) {
            case 0: // 下
                heldItemSprite->setVisible(true);
                heldItemSprite->setScaleX(1.0f);
                heldItemSprite->setPosition(cocos2d::Vec2(34, 16)); // 下方向的位置
                break;
            case 1: // 上
                heldItemSprite->setVisible(false); // 上方向的位置
                break;
            case 2: // 左
                heldItemSprite->setVisible(true);
                heldItemSprite->setScaleX(-1.0f);
                heldItemSprite->setPosition(cocos2d::Vec2(12, 15)); // 左方向的位置
                break;
            case 3: // 右
                heldItemSprite->setVisible(true);
                heldItemSprite->setScaleX(1.0f);
                heldItemSprite->setPosition(cocos2d::Vec2(28, 15)); // 右方向的位置
                break;
            }
        }
    }
}
void User::loadAnimationFrames() {
    const std::string directions[4] = { "down", "up", "left", "right" };

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::string frameName = StringUtils::format("character/user/user_01/user_0_%s_%d.png.", directions[i].c_str(), j);
            auto frame = SpriteFrame::create(frameName, Rect(0, 0, 40, 60));
            if (frame) {
                m_animationFrames[i].pushBack(frame);
            }
            else {
                CCLOG("Failed to load frame: %s", frameName.c_str());
            }
        }
    }
}

void User::moveDown() {
    m_direction = 0; // 设置方向为下
    m_isMoving = true;
    heldItemSprite->setVisible(false);
}
void User::moveLeft() {
    m_direction = 2; // 设置方向为左
    m_isMoving = true;
    heldItemSprite->setVisible(false);
}
void User::moveRight() {
    m_direction = 3; // 设置方向为右
    m_isMoving = true;
}
void User::moveUp() {
    m_direction = 1; // 设置方向为上
    m_isMoving = true;
    heldItemSprite->setVisible(false);
}

void User::toggleInventory() {
    m_isMoving = false;

    if (!inventoryLayer) {
        isInventoryOpen = true;
        inventoryLayer = cocos2d::Layer::create();
        auto background = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 180));
        inventoryLayer->addChild(background);

        const int slotsPerRow = 8;
        const int rows = 3;
        const float slotSize = 30.0f;

        float startX = (Director::getInstance()->getVisibleSize().width - (slotsPerRow * slotSize)) / 2;
        float startY = (Director::getInstance()->getVisibleSize().height - (rows * slotSize)) / 2;

        background->setPosition(0, 0);

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < slotsPerRow; ++col) {
                auto slot = cocos2d::Sprite::create("pack/slot.png");
                if (slot) {
                    slot->setPosition(Vec2(startX + col * slotSize + slotSize / 2, startY + row * slotSize + slotSize / 2));
                    inventoryLayer->addChild(slot);
                }
            }
        }

        auto trashSlot = cocos2d::Sprite::create("pack/slot.png"); 
        if (trashSlot) {
            trashSlot->setPosition(Vec2(startX + slotsPerRow * slotSize + slotSize / 2, startY + slotSize / 2)); // 放在最后一个槽位的右边
            inventoryLayer->addChild(trashSlot);
        }

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < slotsPerRow; ++col) {
                auto slotItems = inventory->getItems(row, col);
                if (!slotItems.empty()) {
                    Item* item = slotItems[0];
                    auto sprite = item->getSprite();

                    if (sprite) {
                        sprite->setPosition(Vec2(startX + col * slotSize + slotSize / 2, startY + row * slotSize + slotSize / 2));
                        inventoryLayer->addChild(sprite);

                        // 获取物品数量
                        int quantity = item->getQuantity();
                        auto quantityLabel = cocos2d::Label::createWithSystemFont(std::to_string(quantity), "Arial", 12);
                        quantityLabel->setPosition(Vec2(startX + col * slotSize + slotSize - 10, startY + row * slotSize + 10));
                        inventoryLayer->addChild(quantityLabel);
                    }
                }
            }
        }

        Director::getInstance()->getRunningScene()->addChild(inventoryLayer, 10, "InventoryLayer");
    }
    else {
        isInventoryOpen = false;
        cocos2d::Scene* currentScene = Director::getInstance()->getRunningScene();
        if (currentScene->getChildByName("InventoryLayer")) {
            currentScene->removeChildByName("InventoryLayer");
        }
        if (currentScene->getChildByName("InventoryBarLayer")) {
            currentScene->removeChildByName("InventoryBarLayer");
        }
        createInventoryBar();
        inventoryLayer = nullptr;
    }
}
void User::onSlotClicked(int row, int col) {
    if (inventoryLayer) {
        // 检查是否点击垃圾桶槽位 
        if (row == 0 && col == 8) {
            // 垃圾桶槽位，删除选中的物品
            Item* selectedItem = getSelectedItem();
            if (selectedItem) {
                // 调用现有的减少数量方法
                reduceSelectedItemQuantity(selectedItem->getQuantity(),false); 
                updateInventoryDisplay(); // 更新背包显示
            }
            return; // 返回，不处理其他槽位
        }

        if (inventory->getItems(row, col).empty()) {
            // 如果当前槽位为空
            if (selectedSlot.first != -1 && selectedSlot.second != -1) {
                if (inventory->moveItems(row, col)) {
                    updateInventoryDisplay(); // 更新显示
                }
            }
        }
        else {
            // 选中当前槽位
            inventory->selectSlot(row, col);
            selectedSlot = { row, col };
            CCLOG("选中槽位: 行: %d, 列: %d", selectedSlot.first, selectedSlot.second);
            Item* selectedItem = getSelectedItem();
            updateInventoryDisplay();
        }
    }
}
void User::updateInventoryDisplay() {
    // 清除当前显示
    inventoryLayer->removeAllChildren();

    // 重新绘制背景
    const int slotsPerRow = 8; // 每行 8 个格子
    const int rows = 3; // 3 行
    const float slotSize = 30.0f; // 每个格子的大小
    float startX = (Director::getInstance()->getVisibleSize().width - (slotsPerRow * slotSize)) / 2;
    float startY = (Director::getInstance()->getVisibleSize().height - (rows * slotSize)) / 2;

    // 绘制背景
    auto background = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 180));
    background->setPosition(0, 0);  // 背景覆盖整个背包层
    inventoryLayer->addChild(background);

    // 创建槽位
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < slotsPerRow; ++col) {
            auto slot = cocos2d::Sprite::create("pack/slot.png");
            if (slot) {
                // 设置格子的绝对位置
                slot->setPosition(Vec2(startX + col * slotSize + slotSize / 2,
                    startY + row * slotSize + slotSize / 2));
                inventoryLayer->addChild(slot);
            }
        }
    }

    auto trashSlot = cocos2d::Sprite::create("pack/slot.png");
    if (trashSlot) {
        trashSlot->setPosition(Vec2(startX + slotsPerRow * slotSize + slotSize / 2, startY + slotSize / 2)); // 放在最后一个槽位的右边
        inventoryLayer->addChild(trashSlot);
    }

    // 重新绘制物品
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < slotsPerRow; ++col) {
            auto slotItems = inventory->getItems(row, col);
            if (!slotItems.empty()) {
                Item* item = slotItems[0]; // 获取第一个物品
                auto sprite = item->getSprite();
                if (sprite) {
                    sprite->setPosition(Vec2(startX + col * slotSize + slotSize / 2,
                        startY + row * slotSize + slotSize / 2));
                    inventoryLayer->addChild(sprite);

                    // 获取并显示物品数量
                    int quantity = item->getQuantity(); // 使用物品数量而不是槽位大小
                    auto quantityLabel = cocos2d::Label::createWithSystemFont(std::to_string(quantity), "Arial", 12);
                    quantityLabel->setPosition(Vec2(startX + col * slotSize + slotSize - 10, // 右下角
                        startY + row * slotSize + 10));
                    inventoryLayer->addChild(quantityLabel);
                }
            }
        }
    }
}

void User::createInventoryBar() {
    // 创建横栏容器
    auto inventoryBarLayer = cocos2d::Layer::create();
    auto background = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 0));
    inventoryBarLayer->addChild(background);

    const int slotsPerRow = 8;
    const float slotSize = 30.0f;

    float startX = (Director::getInstance()->getVisibleSize().width - (slotsPerRow * slotSize)) / 2;
    float startY = 0;

    // 添加槽位
    for (int i = 0; i < slotsPerRow; ++i) {
        auto slot = cocos2d::Sprite::create("pack/slot.png");
        if (slot) {
            slot->setPosition(Vec2(startX + i * slotSize + slotSize / 2, startY + slotSize / 2));
            slot->setTag(i);
            inventoryBarLayer->addChild(slot);
        }
    }

    // 获取背包第0行的物品列表并添加到物品栏
    for (int i = 0; i < slotsPerRow; ++i) {
        auto items = inventory->getItems(0, i);
        if (!items.empty()) {
            Item* item = items[0];
            auto sprite = item->getSprite();

            if (sprite) {
                sprite->setPosition(Vec2(startX + i * slotSize + slotSize / 2, startY + slotSize / 2));
                inventoryBarLayer->addChild(sprite);

                // 获取物品数量
                int quantity = item->getQuantity(); // 获取物品的数量而不是槽位的数量
                auto quantityLabel = cocos2d::Label::createWithSystemFont(std::to_string(quantity), "Arial", 12);
                quantityLabel->setPosition(Vec2(startX + i * slotSize + slotSize - 10, startY + slotSize / 4));
                inventoryBarLayer->addChild(quantityLabel);
            }
        }
    }

    Director::getInstance()->getRunningScene()->addChild(inventoryBarLayer, 10, "InventoryBarLayer");
}
void User::selectItemFromInventory(int index) {

    heldItemSprite->setVisible(false);

    if (index < 0 || index >= 8) {
        return; // 无效的索引，直接返回
    }

    // 获取横栏层
    auto currentBarLayer = Director::getInstance()->getRunningScene()->getChildByName("InventoryBarLayer");
    if (!currentBarLayer) {
        return;
    }

    // 重置所有槽位为 slot.png
    for (int i = 0; i < 8; ++i) {
        auto slotSprite = dynamic_cast<cocos2d::Sprite*>(currentBarLayer->getChildByTag(i));
        if (slotSprite) {
            slotSprite->setTexture("pack/slot.png"); // 恢复为默认状态
        }
    }
    CCLOG("当前选中槽位: 行: %d, 列?: %d", selectedSlot.first, selectedSlot.second);
    CCLOG(" 列?: %d", index);
    // 获取并更改当前选中槽位的纹理
    auto selectedSlotSprite = dynamic_cast<cocos2d::Sprite*>(currentBarLayer->getChildByTag(index));
    if (selectedSlotSprite) {
        CCLOG("Changing texture of slot %d to slot_02.png", index);
        selectedSlotSprite->setTexture("pack/slot_02.png"); // 设为选中状态
    }
    else {
        CCLOG("No slot found for index: %d", index);
    }

    // 在这里获取对应索引的物品
    auto items = inventory->getItems(0, index); 

    if (items.empty()||items[0]->getQuantity()==0) {
        // 处理空情况，例如隐藏 heldItemSprite
        CCLOG("not found，items.size() = %lu", items.size());
        heldItemSprite->setVisible(false);
        return; // 确保不继续执行后续逻辑
    }

    if (!items.empty()) {
        inventory->selectSlot(0, index);
        selectedSlot = { 0, index };
        Item* selectedItem = items[0]; // 获取物品
        if (selectedItem) {
            // 更新 heldItemSprite 的贴图
            heldItemSprite->setTexture(selectedItem->getImagePath());
            heldItemSprite->setVisible(true);
            switch (m_direction) {
            case 0: // 下
                heldItemSprite->setVisible(true);
                heldItemSprite->setScaleX(1.0f);
                heldItemSprite->setPosition(cocos2d::Vec2(34, 16)); // 下方向的位置
                break;
            case 1: // 上
                heldItemSprite->setVisible(false); // 上方向的位置
                break;
            case 2: // 左
                heldItemSprite->setVisible(true);
                heldItemSprite->setScaleX(-1.0f);
                heldItemSprite->setPosition(cocos2d::Vec2(12, 15)); // 左方向的位置
                break;
            case 3: // 右
                heldItemSprite->setVisible(true);
                heldItemSprite->setScaleX(1.0f);
                heldItemSprite->setPosition(cocos2d::Vec2(28, 15)); // 右方向的位置
                break;
            }
        }
    }
}

Item* User::getSelectedItem() {
    if (selectedSlot.first != -1 && selectedSlot.second != -1) {
        auto items = inventory->getItems(selectedSlot.first, selectedSlot.second);
        if (!items.empty()) {
            return items[0]; // 返回选中的物品
        }
    }
    return nullptr;
}

bool User::reduceSelectedItemQuantity(int quantity,bool status) {
 
    if (selectedSlot.first != -1 && selectedSlot.second != -1) {
        // 调用 Inventory 的方法尝试减少数量
        bool success = inventory->reduceItemQuantity(selectedSlot.first, selectedSlot.second, quantity);

        if (success) {
            if (inventoryLayer) {
                updateInventoryDisplay(); // 更新背包显示
            }
            else {
                CCLOG("inventoryLayer 指针为空，不能更新背包显示");
            }
            if (status) {
                cocos2d::Scene* currentScene = Director::getInstance()->getRunningScene();
                if (currentScene->getChildByName("InventoryBarLayer")) {
                    currentScene->removeChildByName("InventoryBarLayer");
                }
                createInventoryBar(); // 调用横栏更新函数
            }

           
        
        }
        else {
            CCLOG("减少物品数量失败: 物品不足或没有选中物品。");
        }
        return success; // 返回减少结果
    }
    return false; // 若未选中物品槽位则返回失败
}

cocos2d::Sprite* User::getHeldItemSprite() const {
    return heldItemSprite; // 返回 heldItemSprite
}

void User::toggleSlotImage() {
    if (slotSprite == nullptr) {
        // 创建并显示槽位图像
        slotSprite = cocos2d::Sprite::create("pack/slot_03.png");
        if (slotSprite) {
            slotSprite->setPosition(Director::getInstance()->getVisibleSize() / 2);
            Director::getInstance()->getRunningScene()->addChild(slotSprite, 10);
            isSlotImageOpen = true;
        }
    }
    else {
        // 如果槽位图像已经存在，则移除它
        isSlotImageOpen = false;
        slotSprite->removeFromParent();
        slotSprite = nullptr; // 清空指针
    }
}