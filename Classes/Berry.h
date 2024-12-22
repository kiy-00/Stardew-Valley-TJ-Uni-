#ifndef __BERRY_H__
#define __BERRY_H__

#include "Crop.h"

class Berry : public Crop {
public:
    static Berry* create(FarmlandTile* tile) {
        Berry* berry = new (std::nothrow) Berry();
        if (berry && berry->init("berry", tile)) {
            berry->autorelease();
            berry->initBerryProperties();
            return berry;
        }
        CC_SAFE_DELETE(berry);
        return nullptr;
    }

protected:
    virtual void initBerryProperties() {
        // 莓果特性配置
        properties.growthRate = 1.2f;
        properties.waterNeed = 60.0f;
        properties.fertilityNeed = 40.0f;

        // 莓果对天气的特殊反应
        properties.weatherModifiers = {
            {"sunny", 1.3f},   // 阳光充足生长更快
            {"cloudy", 1.0f},
            {"rainy", 1.2f},   // 喜欢雨水
            {"snowy", 0.3f},   // 严重受寒
            {"foggy", 0.9f}
        };

        // 莓果的季节特性
        properties.seasonModifiers = {
            {"spring", 1.1f},
            {"summer", 1.4f},  // 夏季生长最好
            {"fall", 0.9f},
            {"winter", 0.4f}   // 冬季生长很差
        };
    }

    // 重写收获方法
    virtual void harvest() override {
        if (!canHarvest()) return;

        // TODO: 添加收获物品到玩家背包
        // 这里需要和物品系统对接

        // 收获后作物消失
        this->removeFromParent();
    }
};

#endif // __BERRY_H__