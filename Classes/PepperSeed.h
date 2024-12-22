#ifndef __PEPPER_SEED_H__
#define __PEPPER_SEED_H__

#include "Item.h"

class PepperSeed : public Item {
public:
    PepperSeed() : Item(
        "Pepper Seed",                          // 名称
        "A seed that grows into pepper, which programmer don't like", // 描述
        "seed",                                // 物品类型
        "crops/pepper/pepper_0.png",          // 图片路径
        1                                      // 初始数量
    ) {
    }

    //未来更改初始种子逻辑
    static PepperSeed* create(int quantity = 50) {
        PepperSeed* seed = new PepperSeed();
        seed->setQuantity(quantity);  // 设置初始数量
        return seed;
    }
};

#endif // __CARROT_SEED_H__