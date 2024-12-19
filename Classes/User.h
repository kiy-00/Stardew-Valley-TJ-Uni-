// User.h
#ifndef __USER_H__
#define __USER_H__

#include "cocos2d.h"
#include "Inventory.h"

class User : public cocos2d::Sprite {
public:
    static User* create(const std::string& name, const std::string& gender, int health, int energy, int money);
    bool init(const std::string& name, const std::string& gender, int health, int energy, int money);

    // 动画相关
    void loadAnimationFrames();
    void updateAnimation(float dt);

    // 移动相关
    void moveDown();
    void moveUp();
    void moveLeft();
    void moveRight();
    void stopMoving() { m_isMoving = false; }

    // 背包系统
    void toggleInventory();
    void createInventoryBar();
    void updateInventoryDisplay();
    void onSlotClicked(int row, int col);
    void selectItemFromInventory(int index);
    bool getIsInventoryOpen() const { return isInventoryOpen; }
    void setIsInventoryOpen(bool open) { isInventoryOpen = open; }

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    const std::string& getGender() const { return m_gender; }
    void setGender(const std::string& gender) { m_gender = gender; }
    const int getMoney() const { return m_money; }
    void setMoney(int money) { m_money = money; }
    int getHealth() const { return m_health; }
    void setHealth(int health) { m_health = health; }
    int getEnergy() const { return m_energy; }
    void setEnergy(int energy) { m_energy = energy; }
    Inventory* getInventory() const { return inventory; }
    std::pair<int, int> getSelectedSlot() const { return selectedSlot; }
    void setSelectedSlot(int first, int second) { selectedSlot = std::make_pair(first, second); }

    int getDirection() const { return m_direction; }
    void setDirection(int direction) { m_direction = direction; }
    bool isMoving() const { return m_isMoving; }
    void setMoving(bool moving) { m_isMoving = moving; }


    // 物品系统
    Item* getSelectedItem();
    bool reduceSelectedItemQuantity(int quantity, bool status);
    cocos2d::Sprite* getHeldItemSprite() const;
   
    //制造系统
    void toggleSlotImage();
    bool getIsSlotImageOpen() const { return isSlotImageOpen; }
    void setIsSlotImageOpen(bool open) { isSlotImageOpen = open; }

   
  

    // 新增：透明度控制
    void updateVisibility(bool isPenetrable) {
        this->setOpacity(isPenetrable ? 128 : 255);
    }

    // 新增：物理系统初始化
    void initPhysics() {
        cocos2d::Size playerSize = this->getContentSize();
        auto physicsBody = cocos2d::PhysicsBody::createBox(
            playerSize,
            cocos2d::PhysicsMaterial(1.0f, 0.0f, 1.0f)
        );

        physicsBody->setDynamic(true);
        physicsBody->setRotationEnable(false);
        physicsBody->setGravityEnable(false);
        physicsBody->setCategoryBitmask(0xFFFFFFFF);
        physicsBody->setCollisionBitmask(0xFFFFFFFF);
        physicsBody->setContactTestBitmask(0xFFFFFFFF);
        physicsBody->setEnabled(true);
        physicsBody->setTag(1);

        this->setPhysicsBody(physicsBody);
    }

private:
    // 基本属性
    std::string m_name;
    std::string m_gender;
    int m_health;
    int m_energy;
    int m_money;

    // 动画相关
    int m_direction;
    bool m_isMoving;
    int m_frameIndex;
    int m_updateCounter;
    cocos2d::Vector<cocos2d::SpriteFrame*> m_animationFrames[4];

    // 背包相关
    Inventory* inventory;
    cocos2d::Layer* inventoryLayer;
    bool isInventoryOpen;
    std::pair<int, int> selectedSlot;

    // 物品相关
    cocos2d::Sprite* heldItemSprite;
    cocos2d::Sprite* slotSprite;
    bool isSlotImageOpen;
};

#endif
