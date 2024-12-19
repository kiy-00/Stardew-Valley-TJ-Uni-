#include "Inventory.h"

Inventory::Inventory() {}

Inventory::~Inventory() {
    for (int row = 0; row < ROWS; ++row) {
        for (int slot = 0; slot < SLOTS_PER_ROW; ++slot) {
            for (Item* item : items[row][slot]) {
                delete item; // 确保释放内存
            }
        }
    }
}

bool Inventory::addItem(Item* item) {
    for (int row = 0; row < ROWS; ++row) {
        for (int slot = 0; slot < SLOTS_PER_ROW; ++slot) {
            // 检查槽位是否还有空间
            if (items[row][slot].size() < MAX_ITEMS_PER_SLOT) {
                if (items[row][slot].empty() || items[row][slot][0]->getName() == item->getName()) {
                    // 如果槽位中存在相同名称的物品，增加其数量
                    if (!items[row][slot].empty()) {
                        int newQuantity = items[row][slot][0]->getQuantity() + item->getQuantity();
                        if (newQuantity > MAX_ITEMS_PER_SLOT) {
                            continue;
                        }
                        items[row][slot][0]->setQuantity(newQuantity);
                    }
                    else {
                        items[row][slot].push_back(item); // 添加物品
                    }
                    return true; // 成功添加物品
                }
            }
        }
    }
    return false; // 没有可用槽位或不符合条件
}

bool Inventory::reduceItemQuantity(int row, int slot, int quantity) {
    if (row < ROWS && slot < SLOTS_PER_ROW) {
        auto& itemsInSlot = items[row][slot];
        if (!itemsInSlot.empty()) {
            Item* item = itemsInSlot[0];
            int currentQuantity = item->getQuantity();
            if (currentQuantity >= quantity && currentQuantity - quantity >= 0) {
                item->setQuantity(currentQuantity - quantity);
                // 如果物品数量减少到0，则清空该槽位
                if (item->getQuantity() == 0) {
                    itemsInSlot.clear();
                }
                return true; // 成功减少数量
            }
        }
    }
    return false; // 不足以减少或槽位无效
}

// 获取指定槽位的物品列表
std::vector<Item*> Inventory::getItems(int row, int slot) const {
    if (row < ROWS && slot < SLOTS_PER_ROW) {
        return items[row][slot]; // 返回指定槽位的物品列表
    }
    return {}; // 返回空列表，表示槽位无效
}
void Inventory::selectSlot(int row, int col) {
    selectedSlot = { row, col }; // 记录选中槽位的坐标
}

bool Inventory::moveItems(int targetRow, int targetCol) {
    if (selectedSlot.first == -1 || selectedSlot.second == -1) {
        return false; // 没有选中槽位
    }

    auto selectedItems = items[selectedSlot.first][selectedSlot.second];

    // 检查目标槽位是否为空
    if (items[targetRow][targetCol].empty()) {
        items[targetRow][targetCol] = selectedItems;  // 移动物品
        items[selectedSlot.first][selectedSlot.second].clear(); // 清空原槽位
    }
    else {
        // 进行交换
        std::swap(items[selectedSlot.first][selectedSlot.second], items[targetRow][targetCol]);
    }

    // 清空选中状态
    selectedSlot = { -1, -1 };
    return true;
}