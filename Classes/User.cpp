#include "User.h"
#include "Item.h"
#include "cocos2d.h"
#include "RenderConstants.h"

USING_NS_CC;

////////////////////////////////////////////////////////////
// 创建 User 对象的工厂方法
// @param name   用户名称
// @param gender 性别
// @param health 健康值
// @param energy 能量值
// @param money  金钱数量
// @return 创建好的 User 指针
////////////////////////////////////////////////////////////
User* User::create(const std::string& name, const std::string& gender, int health, int energy, int money) {
    User* pRet = new(std::nothrow) User();
    if (pRet && pRet->init(name, gender, health, energy, money)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

////////////////////////////////////////////////////////////
// 初始化 User 对象
// 设定基本属性，加载动画帧，初始化 heldItemSprite, inventory 等。
// @param name   用户名称
// @param gender 性别
// @param health 健康值
// @param energy 能量值
// @param money  金钱数量
// @return 初始化是否成功
////////////////////////////////////////////////////////////
bool User::init(const std::string& name, const std::string& gender, int health, int energy, int money) {
    m_name = name;
    m_gender = gender;
    m_health = health;
    m_energy = energy;
    m_money = money;
    m_direction = 0;
    m_isMoving = false;

    // 创建并添加 heldItemSprite
    heldItemSprite = cocos2d::Sprite::create();
    this->addChild(heldItemSprite, 1);
    heldItemSprite->setVisible(false);

    // 初始化背包
    inventory = new Inventory();
    isInventoryOpen = false;

    // 加载角色动画帧
    loadAnimationFrames();

    // 设置默认站立动画帧
    this->setSpriteFrame(m_animationFrames[0].at(0));
    m_frameIndex = 0;
    m_updateCounter = 0;
    // 使用 schedule 定期调用 updateAnimation 来更新动画
    schedule(CC_SCHEDULE_SELECTOR(User::updateAnimation), 0.1f);

    // 创建工具动画精灵
    toolActionSprite = cocos2d::Sprite::create();
    this->addChild(toolActionSprite, 2);
    toolActionSprite->setVisible(false);

    return true;
}

////////////////////////////////////////////////////////////
// 更新动画逻辑，每隔 0.1 秒被调用一次
// 如果正在播放工具动画则跳过更新，否则根据移动与方向更新角色动画帧和物品显示。
////////////////////////////////////////////////////////////
void User::updateAnimation(float dt) {
    if (m_isPerformingToolAction) {
        // 工具动作进行中，跳过普通动画更新
        return;
    }

    m_updateCounter++;
    // 每 4 次调用才切换一帧，降低动画切换频率
    if (m_updateCounter % 4 == 0) {
        if (m_isMoving) {
            // 移动中，循环播放移动动画
            m_frameIndex = (m_frameIndex + 1) % 4;
            heldItemSprite->setVisible(false);
            this->setSpriteFrame(m_animationFrames[m_direction].at(m_frameIndex));
        }
        else {
            // 静止时使用站立帧
            this->setSpriteFrame(m_animationFrames[m_direction].at(0));
            // 检查选中槽位是否有物品
            if (selectedSlot.first != -1 && selectedSlot.second != -1) {
                auto items = inventory->getItems(selectedSlot.first, selectedSlot.second);
                if (!items.empty() && items[0]->getQuantity() > 0) {
                    heldItemSprite->setVisible(true);
                }
                else {
                    heldItemSprite->setVisible(false);
                    return;
                }
            }
            else {
                heldItemSprite->setVisible(false);
                return;
            }
            // 根据方向摆放物品位置与方向
            switch (m_direction) {
            case 0: // 下
                heldItemSprite->setVisible(true);
                heldItemSprite->setScaleX(1.0f);
                heldItemSprite->setPosition(Vec2(34, 16));
                break;
            case 1: // 上
                heldItemSprite->setVisible(true);
                heldItemSprite->setScaleX(1.0f);
                heldItemSprite->setPosition(Vec2(36, 20));
                break;
            case 2: // 左
                heldItemSprite->setVisible(true);
                heldItemSprite->setScaleX(-1.0f);
                heldItemSprite->setPosition(Vec2(12, 15));
                break;
            case 3: // 右
                heldItemSprite->setVisible(true);
                heldItemSprite->setScaleX(1.0f);
                heldItemSprite->setPosition(Vec2(28, 15));
                break;
            }
        }
    }
}

////////////////////////////////////////////////////////////
// 加载角色行走动画帧
// 从对应路径加载上下左右4个方向，每方向4帧的行走动画。
////////////////////////////////////////////////////////////
void User::loadAnimationFrames() {
    const std::string directions[4] = { "down", "up", "left", "right" };
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::string frameName = StringUtils::format("character/user/user_01/user_0_%s_%d.png", directions[i].c_str(), j);
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

////////////////////////////////////////////////////////////
// 获取工具动画帧
// 根据工具名和方向加载对应的plist并从中获取两帧动画。
// @param toolName 工具名称（无扩展名）
// @param direction 方向(0下,1上,2左,3右)
// @return 包含两帧工具动画的 Vector<SpriteFrame*>
////////////////////////////////////////////////////////////
Vector<SpriteFrame*> User::getToolAnimationFrames(const std::string& toolName, int direction) {
    Vector<SpriteFrame*> frames;
    std::string directionName;
    switch (direction) {
    case 0: directionName = "down"; break;
    case 1: directionName = "up"; break;
    case 2: directionName = "left"; break;
    case 3: directionName = "right"; break;
    default: directionName = "down"; break;
    }

    // 加载对应方向的plist文件
    std::string plistFile = StringUtils::format("tool/plist/%s_%s.plist", toolName.c_str(), directionName.c_str());
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistFile);
    CCLOG("Loaded plist file: %s", plistFile.c_str());

    // 尝试加载两帧动画
    for (int i = 0; i < 2; ++i) {
        std::string frameName = StringUtils::format("%s_%s_%d.png", toolName.c_str(), directionName.c_str(), i);
        CCLOG("Attempting to load frame: %s", frameName.c_str());
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            frames.pushBack(frame);
            CCLOG("Successfully loaded frame: %s", frameName.c_str());
        }
        else {
            CCLOG("Failed to load frame: %s", frameName.c_str());
        }
    }

    return frames;
}

////////////////////////////////////////////////////////////
// 以下 moveXXX 函数用于设置角色移动方向并开始移动动画
// 同时隐藏 heldItemSprite 防止移动过程中物品显示不自然
////////////////////////////////////////////////////////////
void User::moveDown() {
    m_direction = 0;
    m_isMoving = true;
    heldItemSprite->setVisible(false);
}
void User::moveLeft() {
    m_direction = 2;
    m_isMoving = true;
    heldItemSprite->setVisible(false);
}
void User::moveRight() {
    m_direction = 3;
    m_isMoving = true;
    heldItemSprite->setVisible(false);
}
void User::moveUp() {
    m_direction = 1;
    m_isMoving = true;
    heldItemSprite->setVisible(false);
}

////////////////////////////////////////////////////////////
// toggleInventory 切换背包显示状态
// 如果当前背包未显示则创建背包UI层并显示，
// 如果已显示则移除背包UI层。
////////////////////////////////////////////////////////////
void User::toggleInventory() {
    m_isMoving = false;

    if (!inventoryLayer) {
        isInventoryOpen = true;
        inventoryLayer = cocos2d::Layer::create();

        const int slotsPerRow = 8;
        const int rows = 3;
        const float slotSize = 30.0f;

        // 设置背包整体位置相对于玩家
        inventoryLayer->setPosition(this->getPosition() + Vec2(-125, 100));

        // 创建半透明背景
        auto background = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 180));
        background->setContentSize(cocos2d::Size(slotsPerRow * slotSize + slotSize, rows * slotSize + slotSize));
        background->setPosition(-slotSize / 2, -slotSize / 2);
        inventoryLayer->addChild(background);

        // 创建背包槽位
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < slotsPerRow; ++col) {
                auto slot = cocos2d::Sprite::create("pack/slot.png");
                if (slot) {
                    slot->setPosition(Vec2(col * slotSize + slotSize / 2, row * slotSize + slotSize / 2));
                    inventoryLayer->addChild(slot);
                }
            }
        }

        // 创建丢弃槽（在右侧）
        auto trashSlot = cocos2d::Sprite::create("pack/garbage.png");
        if (trashSlot) {
            trashSlot->setPosition(Vec2(slotsPerRow * slotSize + slotSize / 2, slotSize / 2));
            inventoryLayer->addChild(trashSlot);
        }

        // 显示物品
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < slotsPerRow; ++col) {
                auto slotItems = inventory->getItems(row, col);
                if (!slotItems.empty()) {
                    Item* item = slotItems[0];
                    auto sprite = item->getSprite();
                    if (sprite) {
                        sprite->setPosition(Vec2(col * slotSize + slotSize / 2, row * slotSize + slotSize / 2));
                        inventoryLayer->addChild(sprite);

                        int quantity = item->getQuantity();
                        auto quantityLabel = cocos2d::Label::createWithSystemFont(std::to_string(quantity), "Arial", 12);
                        quantityLabel->setPosition(Vec2(col * slotSize + slotSize - 10, row * slotSize + 10));
                        inventoryLayer->addChild(quantityLabel);
                    }
                }
            }
        }
        Director::getInstance()->getRunningScene()->addChild(inventoryLayer, FOUR, "InventoryLayer");
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

////////////////////////////////////////////////////////////
// onSlotClicked 响应背包槽位点击事件
// 根据点击的槽位选择物品或移动物品。
////////////////////////////////////////////////////////////
void User::onSlotClicked(int row, int col) {
    if (!inventoryLayer) return;

    // 检查是否点击垃圾桶槽位 
    if (row == 0 && col == 8) {
        // 如果当前有选中物品，则将其全部删除
        if (selectedSlot.first != -1 && selectedSlot.second != -1) {
            Item* selectedItem = getSelectedItem();
            if (selectedItem) {
                reduceSelectedItemQuantity(selectedItem->getQuantity(), false);
                // 重置选中槽位
                selectedSlot = { -1, -1 };
                heldItemSprite->setVisible(false);
                updateInventoryDisplay();
            }
        }
        return;
    }

    // 获取当前槽位的物品列表
    auto slotItems = inventory->getItems(row, col);
    bool slotEmpty = slotItems.empty() || slotItems[0]->getQuantity() == 0;

    // 情况1：当前未选中任何物品
    if (selectedSlot.first == -1 && selectedSlot.second == -1) {
        if (!slotEmpty) {
            // 选中当前槽位的物品
            inventory->selectSlot(row, col);
            selectedSlot = { row, col };
            updateHeldItemSprite();
            updateInventoryDisplay();
        }
        return;
    }
    // 情况2：当前已经选中一个物品
    if (slotEmpty) {
        // 将选中物品移动到该空槽位
        if (inventory->moveItems(row, col)) {
            selectedSlot = { -1, -1 };
            heldItemSprite->setVisible(false);
            updateInventoryDisplay();
        }
        return;
    }
    else {
        // 选中新的物品
        inventory->selectSlot(row, col);
        selectedSlot = { row, col };
        updateHeldItemSprite();
        updateInventoryDisplay();
    }
}

////////////////////////////////////////////////////////////
// updateHeldItemSprite 更新持有物品显示
// 根据当前是否选中物品更新显示。
////////////////////////////////////////////////////////////
void User::updateHeldItemSprite() {
    Item* selectedItem = getSelectedItem();
    if (selectedItem) {
        heldItemSprite->setTexture(selectedItem->getImagePath());
        heldItemSprite->setVisible(true);

        // 根据当前方向设置物品的位置和朝向
        switch (m_direction) {
        case 0: // 下
            heldItemSprite->setScaleX(1.0f);
            heldItemSprite->setPosition(Vec2(34, 16));
            break;
        case 1: // 上
            heldItemSprite->setScaleX(1.0f);
            heldItemSprite->setPosition(Vec2(36, 20));
            break;
        case 2: // 左
            heldItemSprite->setScaleX(-1.0f);
            heldItemSprite->setPosition(Vec2(12, 15));
            break;
        case 3: // 右
            heldItemSprite->setScaleX(1.0f);
            heldItemSprite->setPosition(Vec2(28, 15));
            break;
        }
    }
    else {
        heldItemSprite->setVisible(false);
    }
}

////////////////////////////////////////////////////////////
// updateInventoryDisplay 更新背包界面显示
// 根据当前背包物品信息重新绘制背包UI。
////////////////////////////////////////////////////////////
void User::updateInventoryDisplay() {
    if (!inventoryLayer) return;

    inventoryLayer->removeAllChildren();  // 清除当前显示的所有内容

    const int slotsPerRow = 8;
    const int rows = 3;
    const float slotSize = 30.0f;

    // 绘制背景
    auto background = cocos2d::LayerColor::create(cocos2d::Color4B(0, 0, 0, 180));
    background->setContentSize(cocos2d::Size(slotsPerRow * slotSize + slotSize, rows * slotSize + slotSize));
    background->setPosition(-slotSize / 2, -slotSize / 2);
    inventoryLayer->addChild(background);

    // 绘制背包槽位
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < slotsPerRow; ++col) {
            auto slot = cocos2d::Sprite::create("pack/slot.png");
            slot->setPosition(Vec2(col * slotSize + slotSize / 2, row * slotSize + slotSize / 2));
            inventoryLayer->addChild(slot);
        }
    }

    // 绘制垃圾桶槽位
    auto trashSlot = cocos2d::Sprite::create("pack/garbage.png");
    trashSlot->setPosition(Vec2(slotsPerRow * slotSize + slotSize / 2, slotSize / 2));
    inventoryLayer->addChild(trashSlot);

    // 重新绘制物品
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < slotsPerRow; ++col) {
            auto slotItems = inventory->getItems(row, col);
            if (!slotItems.empty()) {
                Item* item = slotItems[0];
                auto sprite = item->getSprite();
                sprite->setPosition(Vec2(col * slotSize + slotSize / 2, row * slotSize + slotSize / 2));
                inventoryLayer->addChild(sprite);

                int quantity = item->getQuantity();
                auto quantityLabel = cocos2d::Label::createWithSystemFont(std::to_string(quantity), "Arial", 12);
                quantityLabel->setPosition(Vec2(col * slotSize + slotSize - 10, row * slotSize + 10));
                inventoryLayer->addChild(quantityLabel);
            }
        }
    }
}

////////////////////////////////////////////////////////////
// createInventoryBar 创建横栏物品栏
// 显示背包第0行的物品在屏幕下方的横栏中。
////////////////////////////////////////////////////////////
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
    Director::getInstance()->getRunningScene()->addChild(inventoryBarLayer, FOUR, "InventoryBarLayer");
}

////////////////////////////////////////////////////////////
// selectItemFromInventory 从横栏选中物品槽位
// 显示选中物品的图标（heldItemSprite）并更新状态。
////////////////////////////////////////////////////////////
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

    if (items.empty() || items[0]->getQuantity() == 0) {
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
                heldItemSprite->setVisible(true);
                heldItemSprite->setScaleX(1.0f);
                heldItemSprite->setPosition(cocos2d::Vec2(36, 20)); // 上方向的位置
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

////////////////////////////////////////////////////////////
// getSelectedItem 获取当前选中的物品
// @return 当前选中槽位的物品指针，如果没有选中返回nullptr
////////////////////////////////////////////////////////////
Item* User::getSelectedItem() {
    if (selectedSlot.first != -1 && selectedSlot.second != -1) {
        auto items = inventory->getItems(selectedSlot.first, selectedSlot.second);
        if (!items.empty()) {
            return items[0];
        }
    }
    return nullptr;
}

////////////////////////////////////////////////////////////
// reduceSelectedItemQuantity 减少选中物品数量
// @param quantity 要减少的数量
// @param status 是否需要更新横栏
// @return 是否成功减少数量
////////////////////////////////////////////////////////////
bool User::reduceSelectedItemQuantity(int quantity, bool status) {

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

////////////////////////////////////////////////////////////
// getHeldItemSprite 获取heldItemSprite精灵指针
// 用于显示当前手持物品的图标。
////////////////////////////////////////////////////////////
cocos2d::Sprite* User::getHeldItemSprite() const {
    return heldItemSprite; // 返回 heldItemSprite
}

////////////////////////////////////////////////////////////
// toggleSlotImage 显示或隐藏槽位图像
// 如果 slotSprite 为空则创建显示，否则移除
////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////
// @brief  检查并拾取玩家附近范围内的物品
// @param  items        场景中所有绑定了 Item 对象的 Sprite 列表
// @param  pickupRadius 拾取范围半径（以像素为单位）
// 
// @details
// 遍历给定的 Sprite 列表，检测每个物品与玩家之间的距离，
// 如果物品在拾取范围内，则将其对应的 Item 实例添加到玩家的背包，
// 并从场景中移除该物品的 Sprite。
// 
// @note
// - 每个物品的 Sprite 必须绑定有效的 Item 对象作为 UserData。
// - 物品拾取后，Item 实例会被添加到背包中，Sprite 从场景中移除。
////////////////////////////////////////////////////////////
void User::pickupNearbyItems(const std::vector<cocos2d::Sprite*>& items, float pickupRadius) {
	Vec2 playerPos = this->getPosition();

	for (auto sprite : items) {
		if (!sprite) continue;

		// 检测物品是否在拾取范围内
		Vec2 itemPos = sprite->getPosition();
		float distance = playerPos.distance(itemPos);
		if (distance <= pickupRadius) {
			// 获取 Sprite 绑定的 Item 对象
			void* userData = sprite->getUserData();
			if (!userData) continue;

			// 显式转换为 Item*
			Item* item = static_cast<Item*>(userData); 
			if (item) { 
				// 将物品加入背包
				inventory->addItem(item);

				// 从场景中移除物品
				sprite->removeFromParent();
			}
		}
	}
}


////////////////////////////////////////////////////////////
// performAction 通用动作入口函数
// 据物品类型执行不同逻辑
////////////////////////////////////////////////////////////
void User::performAction(FarmMapManager* farmMapManager) {
    Item* currentItem = getSelectedItem();
    if (!currentItem) {
        CCLOG("No item selected.");
        return;
    }

    std::string itemType = currentItem->getItemType();
    if (itemType == "tool") {
        performToolAction();
    }
    else if (itemType == "seed") {
        plantSeed();
    }
    else if (itemType == "rod") {
        if (farmMapManager->isFishingSpot(getPosition())) {
            startFishing();
        }
    }
    else {
        CCLOG("No specific action defined for item type: %s", itemType.c_str());
    }
}

////////////////////////////////////////////////////////////
// performToolAction 执行工具动作动画
// 当角色手持工具并点击鼠标左键时播放工具动画。
// 播放期间隐藏heldItemSprite，结束后再显示。
////////////////////////////////////////////////////////////
void User::performToolAction() {
    Item* currentItem = getSelectedItem();
    if (!currentItem || currentItem->getItemType().find("tool") == std::string::npos) {
        CCLOG("No valid tool selected.");
        return;
    }

    m_isMoving = false;
    m_isPerformingToolAction = true; // 开始工具动作
    if (heldItemSprite) {
        heldItemSprite->setVisible(false); // 隐藏手持道具
    }

    std::string toolImagePath = currentItem->getImagePath();
    std::string toolName;
    size_t lastSlash = toolImagePath.find_last_of('/');
    size_t dotPos = toolImagePath.find_last_of('.');
    if (lastSlash != std::string::npos && dotPos != std::string::npos && dotPos > lastSlash) {
        toolName = toolImagePath.substr(lastSlash + 1, dotPos - (lastSlash + 1));
    }
    else {
        CCLOG("Failed to parse tool name.");
        m_isPerformingToolAction = false;
        return;
    }

    auto frames = getToolAnimationFrames(toolName, m_direction);
    if (frames.empty()) {
        CCLOG("No tool frames found for %s", toolName.c_str());
        m_isPerformingToolAction = false;
        return;
    }

    toolActionSprite->setVisible(true);
    toolActionSprite->setSpriteFrame(frames.front());
    toolActionSprite->setAnchorPoint(Vec2(0.5f, 0.5f));
    toolActionSprite->setPosition(Vec2(0, 0));

    if (toolName == "kettle") {
        switch (m_direction) {
        case 0: toolActionSprite->setPosition(Vec2(30, 16)); break; // 下
        case 1: toolActionSprite->setPosition(Vec2(32, 18)); break; // 上
        case 2: toolActionSprite->setPosition(Vec2(8, 20));  break; // 左
        case 3: toolActionSprite->setPosition(Vec2(32, 20)); break; // 右
        }
    }
    else {
        switch (m_direction) {
        case 0: toolActionSprite->setPosition(Vec2(30, 16)); break; // 下
        case 1: toolActionSprite->setPosition(Vec2(30, 24)); break; // 上
        case 2: toolActionSprite->setPosition(Vec2(8, 20));  break; // 左
        case 3: toolActionSprite->setPosition(Vec2(32, 20)); break; // 右
        }
        toolActionSprite->setScale(1.5f);
    }

    Animation* animation = Animation::createWithSpriteFrames(frames, 0.2f);
    Animate* animate = Animate::create(animation);

    auto callback = CallFunc::create([this]() {
        toolActionSprite->setVisible(false);
        toolActionSprite->setScale(1.0f); // 动画结束后恢复原始大小
        if (heldItemSprite) {
            heldItemSprite->setVisible(true);
        }
        m_isPerformingToolAction = false;
        CCLOG("Tool action animation completed.");
        });


    toolActionSprite->stopAllActions();
    toolActionSprite->runAction(Sequence::create(animate, callback, nullptr));
    CCLOG("Tool action animation started for tool: %s", toolName.c_str());
}

////////////////////////////////////////////////////////////
// plantSeed 执行种植逻辑
////////////////////////////////////////////////////////////
void User::plantSeed() {
    CCLOG("Planting a seed at current location.");
}


////////////////////////////////////////////////////////////
// startFishing 执行钓鱼逻辑
// 钓鱼小游戏 + 随机挑选鱼 + 按 G 加入背包
////////////////////////////////////////////////////////////
void User::startFishing() {
	CCLOG("Starting fishing game in current scene.");

	// 创建 FishingLayer
	auto fishingLayer = FishingLayer::create();

	// 设置小游戏结束时的回调
	fishingLayer->onGameEnd = [weakFishingLayer = fishingLayer, this](bool success, const FishInfo& fish) {
		auto fishingLayer = weakFishingLayer; // 恢复强引用
		if (!fishingLayer) return;            // 若 FishingLayer 已销毁，则跳过
		if (!success) return;                 // 如果小游戏失败，不显示任何东西

        // 根据当前季节或天气筛选鱼
        std::vector<FishInfo> allFish = FishDataFactory::createFishData();

        // 随机挑选一条鱼
        int randomIndex = RandomHelper::random_int(0, (int)allFish.size() - 1);
        FishInfo candidateFish = allFish[randomIndex];

		auto currentScene = Director::getInstance()->getRunningScene();
		if (!currentScene) return;

		// 创建鱼的 Sprite
		auto fishSprite = Sprite::create(candidateFish.spriteName);

		// 设置鱼的显示位置在人物上方
		auto playerPos = this->getPosition();
		fishSprite->setPosition(playerPos + Vec2(20, 10));
		fishSprite->setScale(1.5f);

		// 创建 FishItem 并绑定到 Sprite
		FishItem* fishItem = new FishItem(candidateFish.name, "A freshly caught fish.", candidateFish.spriteName);
        fishSprite->setUserData(static_cast<void*>(fishItem));

		// 将鱼图添加到场景中
		currentScene->addChild(fishSprite, SIX);
		};

	// 设置 FishingLayer 的位置在玩家左侧
	auto playerPosition = this->getPosition();
	fishingLayer->setPosition(playerPosition - Vec2(120, 0));

	// 缩小整个 FishingLayer 场景为 50%
	fishingLayer->setScale(0.5f);

	// 将 FishingLayer 添加到当前场景
	auto currentScene = cocos2d::Director::getInstance()->getRunningScene();
	currentScene->addChild(fishingLayer, SIX);
}
