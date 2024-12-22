#pragma once
// FarmlandTile.h
#pragma once
#include "cocos2d.h"

USING_NS_CC;

class FarmlandTile : public cocos2d::Sprite {
public:
    enum class State {
        EMPTY,          
        TILLED,         
        WATERED,        
        FERTILIZED      
    };

    static FarmlandTile* create(const Vec2& tilePos);

    bool init(const Vec2& position);
    bool initWithTilePosition(const Vec2& tilePos);

   
    void till();       
    void water();      
    void fertilize();  
    void resetState(); 

    static void setTileSize(float size);

    static float getTileSize() {
        return tileSize;
    }


    State getState() const { return currentState; }

    float getMoisture() const { return moisture; }
    float getFertility() const { return fertility; }


    void update(float dt);

    // 转换瓦片坐标到世界坐标
    static Vec2 tileToWorldPosition(const Vec2& tilePos) {
        auto tileSize = FarmlandTile::getTileSize();
        return Vec2(
            (tilePos.x + 0.5f) * tileSize,
            (tilePos.y + 0.5f) * tileSize
        );
    }

private:
    State currentState;
    float moisture;    
    float fertility;   

    static float tileSize;

    // ������ʾ
    void updateSprite();
};