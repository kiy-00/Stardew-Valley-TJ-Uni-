#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include <vector>
#include "Item.h"

const int ROWS = 3;        // 行数
const int SLOTS_PER_ROW = 8; // 每行槽位数
const int MAX_ITEMS_PER_SLOT = 5; // 每个槽位最多放的物品数

class Inventory : public cocos2d::Node {
public:
    Inventory(); // 构造函数声明
    ~Inventory(); // 析构函数声明

    bool addItem(Item* item); // 返回布尔值指示是否添加成功
    bool reduceItemQuantity(int row, int slot, int quantity);
    std::vector<Item*> getItems(int row, int slot) const; // 获取指定槽位的物品列表
    void selectSlot(int row, int col);
    bool Inventory::moveItems(int targetRow, int targetCol);
  

private:
    std::vector<Item*> items[ROWS][SLOTS_PER_ROW]; // 存储物品的二维数组
    std::pair<int, int> selectedSlot = { -1, -1 };// 选中槽位的坐标
};

#endif