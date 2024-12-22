#ifndef __PEPPER_H__
#define __PEPPER_H__

#include "Crop.h"

class Pepper : public Crop {
public:
    static Pepper* create(FarmlandTile* tile) {
        Pepper* pepper = new (std::nothrow) Pepper();
        if (pepper && pepper->init("pepper", tile)) {
            pepper->autorelease();
            pepper->initPepperProperties();
            return pepper;
        }
        CC_SAFE_DELETE(pepper);
        return nullptr;
    }

protected:
    virtual void initPepperProperties() {
        // 辣椒特性
        properties.growthRate = 1.3f;
        properties.waterNeed = 70.0f;
        properties.fertilityNeed = 50.0f;

        // 辣椒对天气的特殊反应
        properties.weatherModifiers = {
            {"sunny", 1.4f},
            {"cloudy", 1.1f},
            {"rainy", 1.1f},
            {"snowy", 0.3f},
            {"foggy", 0.8f}
        };

        // 辣椒的季节特性
        properties.seasonModifiers = {
            {"spring", 0.9f},
            {"summer", 1.4f},
            {"fall", 1.0f},
            {"winter", 0.1f}
        };
    }

    // 重写收获方法
    virtual void harvest() override {
        if (!canHarvest()) return;

        // TODO: 辣椒的收获逻辑，如给玩家背包+1辣椒
        // 这里需要和物品系统对接

        // 收获后作物消失
        this->removeFromParent();
    }
};

#endif // __PEPPER_H__
