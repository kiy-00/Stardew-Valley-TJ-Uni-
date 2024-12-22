#ifndef __BERRY_SEED_H__
#define __BERRY_SEED_H__

#include "Item.h"

class BerrySeed : public Item {
public:
    BerrySeed() : Item(
        "Berry Seed",                          // 名称
        "A seed that grows into sweet berries", // 描述
        "seed",                                // 物品类型
        "crops/berry/berry_0.png",          // 图片路径
        1                                      // 初始数量
    ) {}

    //未来更改初始种子逻辑
    static BerrySeed* create(int quantity = 50) {
        BerrySeed* seed = new BerrySeed();
        seed->setQuantity(quantity);  // 设置初始数量
        return seed;
    }
};

#endif // __BERRY_SEED_H__