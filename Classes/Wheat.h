#ifndef __WHEAT_H__
#define __WHEAT_H__

#include "Crop.h"

class Wheat : public Crop {
public:
    static Wheat* create(FarmlandTile* tile) {
        Wheat* wheat = new (std::nothrow) Wheat();
        if (wheat && wheat->init("wheat", tile)) {
            wheat->autorelease();
            wheat->initWheatProperties();
            return wheat;
        }
        CC_SAFE_DELETE(wheat);
        return nullptr;
    }

protected:
    virtual void initWheatProperties() {
        // 小麦特性
        properties.growthRate = 1.1f;
        properties.waterNeed = 40.0f;
        properties.fertilityNeed = 35.0f;

        // 小麦对天气的特殊反应
        properties.weatherModifiers = {
            {"sunny", 1.3f},
            {"cloudy", 1.1f},
            {"rainy", 1.0f},
            {"snowy", 0.5f},
            {"foggy", 0.9f}
        };

        // 小麦的季节特性
        properties.seasonModifiers = {
            {"spring", 1.0f},
            {"summer", 1.3f},
            {"fall", 1.1f},
            {"winter", 0.3f}
        };
    }

    // 重写收获方法
    virtual void harvest() override {
        if (!canHarvest()) return;

        // TODO: 小麦的收获逻辑，如给玩家背包+若干麦穗
        // 这里需要和物品系统对接

        // 收获后作物消失
        this->removeFromParent();
    }
};

#endif // __WHEAT_H__
