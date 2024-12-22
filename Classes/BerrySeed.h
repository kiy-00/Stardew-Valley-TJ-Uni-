#ifndef __BERRY_SEED_H__
#define __BERRY_SEED_H__

#include "Item.h"
#include "Berry.h"
#include "FarmlandTile.h"

class BerrySeed : public Item {
public:
    BerrySeed() : Item(
        "Berry Seed",                          // 名称
        "A seed that grows into sweet berries", // 描述
        "seed",                                // 物品类型
        "crops/berry/berry_0.png",          // 图片路径
        1                                      // 初始数量
    ) {}

    static BerrySeed* create() {
        return new BerrySeed();
    }
};

#endif // __BERRY_SEED_H__