#ifndef __WHEAT_SEED_H__
#define __WHEAT_SEED_H__

#include "Item.h"

class WheatSeed : public Item {
public:
    WheatSeed() : Item(
        "Wheat Seed",                          // 名称
        "A seed that grows into wheats, which is a important crop", // 描述
        "seed",                                // 物品类型
        "crops/wheat/wheat_0.png",          // 图片路径
        1                                      // 初始数量
    ) {
    }

    //未来更改初始种子逻辑
    static WheatSeed* create(int quantity = 50) {
        WheatSeed* seed = new WheatSeed();
        seed->setQuantity(quantity);  // 设置初始数量
        return seed;
    }
};

#endif // __CARROT_SEED_H__