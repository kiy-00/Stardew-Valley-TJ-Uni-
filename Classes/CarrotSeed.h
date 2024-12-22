#ifndef __CARROT_SEED_H__
#define __CARROT_SEED_H__

#include "Item.h"

class CarrotSeed : public Item {
public:
    CarrotSeed() : Item(
        "Carrot Seed",                          // 名称
        "A seed that grows into health carrots", // 描述
        "seed",                                // 物品类型
        "crops/carrot/carrot_0.png",          // 图片路径
        1                                      // 初始数量
    ) {
    }

    //未来更改初始种子逻辑
    static CarrotSeed* create(int quantity = 50) {
        CarrotSeed* seed = new CarrotSeed();
        seed->setQuantity(quantity);  // 设置初始数量
        return seed;
    }
};

#endif // __CARROT_SEED_H__