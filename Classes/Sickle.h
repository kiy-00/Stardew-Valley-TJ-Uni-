#pragma once
#ifndef __SICKLE_H__
#define __SICKLE_H__
#include "ITool.h" 
class Sickle : public ITool {
public:
    bool useTool(Farmland* land) override {
        Crop* crop = land->getCrop();
        if (crop && crop->isDead()) {
            // 清除死亡作物
            land->removeCrop();
            // 可以添加音效或动画效果
            CCLOG("Dead crop cleared!");
        }
        else {
            CCLOG("No dead crop to clear!");
        }
    }

    ToolType getToolName() const override {
        return ToolType::SICKLE;
    }
};
#endif