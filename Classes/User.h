#ifndef __USER_H__
#define __USER_H__

#include "cocos2d.h"
#include "Inventory.h"

class User : public cocos2d::Sprite {
public:
    static User* create(const std::string& name, const std::string& gender, int health, int energy, int money); 

    virtual bool init(const std::string& name, const std::string& gender, int health, int energy, int money); 


    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void updateAnimation(float dt);
  
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
    bool reduceSelectedItemQuantity(int quantity,bool status);
    std::pair<int, int> getSelectedSlot() const {return selectedSlot; }
    void setSelectedSlot(int first, int second) { selectedSlot = std::make_pair(first, second);  }

    int getDirection() const { return m_direction; }
    void setDirection(int direction) { m_direction = direction; }
    bool isMoving() const { return m_isMoving; }
    void setMoving(bool moving) { m_isMoving = moving; }

    //背包
    void toggleInventory();
    void onSlotClicked(int row, int col); // 响应槽位点击
    void updateInventoryDisplay();
    bool getIsInventoryOpen() const {return isInventoryOpen;}
    void setIsInventoryOpen(bool open) {isInventoryOpen = open; }
    //横栏
    void createInventoryBar();
    void selectItemFromInventory(int index);
    //获取选中物体
    Item* getSelectedItem();
    cocos2d::Sprite* getHeldItemSprite() const;

    //制造系统
    void toggleSlotImage();
    bool getIsSlotImageOpen() const {  return isSlotImageOpen; }
    void setIsSlotImageOpen(bool open) {isSlotImageOpen = open; }

private:
    std::string m_name;
    std::string m_gender;
    int m_health;  
    int m_energy;  
    int m_money;

    cocos2d::Sprite* heldItemSprite;
    Inventory* inventory;

    cocos2d::Sprite* slotSprite = nullptr;

    int m_direction;
    bool m_isMoving;

    int m_frameIndex;
    cocos2d::Vector<cocos2d::SpriteFrame*> m_animationFrames[4]; // 4 animations for 4 direction

    bool isInventoryOpen;
    const float slotSize = 30.0f;
    cocos2d::Layer* inventoryLayer = nullptr; // 背包层指针
    std::pair<int, int> selectedSlot = { -1, -1 };

    bool isSlotImageOpen = false;

    int m_updateCounter; // 用于控制动画帧率的计数器

    void loadAnimationFrames();

};

#endif // __USER_H__#pragma once
