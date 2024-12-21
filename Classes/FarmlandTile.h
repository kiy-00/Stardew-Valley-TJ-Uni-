#pragma once
// FarmlandTile.h
#pragma once
#include "cocos2d.h"

USING_NS_CC;

class FarmlandTile : public cocos2d::Node {
public:
    enum class State {
        EMPTY,          // 未耕种
        TILLED,         // 已耕地
        WATERED,        // 已浇水
        FERTILIZED      // 已施肥
    };

    static FarmlandTile* create(const Vec2& position);

    bool init(const Vec2& position);

    // 状态转换方法
    void till();       // 耕地
    void water();      // 浇水
    void fertilize();  // 施肥
    void resetState(); // 重置状态

    static void setTileSize(float size);

    // 获取当前状态
    State getState() const { return currentState; }

    // 获取土壤湿度/肥力等属性
    float getMoisture() const { return moisture; }
    float getFertility() const { return fertility; }

    // 更新状态（例如水分随时间减少）
    void update(float dt);

private:
    State currentState;
    float moisture;    // 土壤湿度 0-100
    float fertility;   // 肥力 0-100

    static float tileSize;

    // 更新显示
    void updateSprite();
};