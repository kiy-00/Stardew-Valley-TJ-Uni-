#ifndef __CARROT_H__
#define __CARROT_H__

#include "Crop.h"

class Carrot : public Crop {
public:
    static Carrot* create(FarmlandTile* tile) {
        Carrot* carrot = new (std::nothrow) Carrot();
        if (carrot && carrot->init("carrot", tile)) {
            carrot->autorelease();
            carrot->initCarrotProperties();
            return carrot;
        }
        CC_SAFE_DELETE(carrot);
        return nullptr;
    }

protected:
    virtual void initCarrotProperties() {
        // 胡萝卜特性
        properties.growthRate = 1.0f;
        properties.waterNeed = 50.0f;
        properties.fertilityNeed = 30.0f;

        // 胡萝卜对天气的特殊反应
        properties.weatherModifiers = {
            {"sunny", 1.2f},
            {"cloudy", 1.0f},
            {"rainy", 1.3f},
            {"snowy", 0.2f},
            {"foggy", 0.8f}
        };

        // 胡萝卜的季节特性
        properties.seasonModifiers = {
            {"spring", 1.2f},
            {"summer", 1.0f},
            {"fall", 0.8f},
            {"winter", 0.3f}
        };
    }

    // 重写收获方法
    virtual void harvest() override {
        if (!canHarvest()) return;

        // TODO: 胡萝卜的收获逻辑，如给玩家背包+1胡萝卜
        // 这里需要和物品系统对接

        // 收获后作物消失
        this->removeFromParent();
    }
};

#endif // __CARROT_H__
