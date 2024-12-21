// FarmlandTile.cpp
#include "FarmlandTile.h"

float FarmlandTile::tileSize = 32.0f;

FarmlandTile* FarmlandTile::create(const Vec2& position) {
    FarmlandTile* tile = new (std::nothrow) FarmlandTile();
    if (tile && tile->init(position)) {
        tile->autorelease();
        return tile;
    }
    CC_SAFE_DELETE(tile);
    return nullptr;
}

bool FarmlandTile::init(const Vec2& position) {
    if (!Node::init()) {
        return false;
    }

    currentState = State::EMPTY;
    moisture = 0.0f;
    fertility = 0.0f;

    this->setPosition(position);
    updateSprite();



    return true;
}

void FarmlandTile::till() {
    if (currentState == State::EMPTY) {
        currentState = State::TILLED;
        updateSprite();
    }
}

void FarmlandTile::water() {
    if (currentState == State::TILLED || currentState == State::FERTILIZED) {
        moisture = 100.0f;
        currentState = State::WATERED;
        updateSprite();
    }
}

void FarmlandTile::fertilize() {
    if (currentState == State::TILLED || currentState == State::WATERED) {
        fertility = 100.0f;
        currentState = State::FERTILIZED;
        updateSprite();
    }
}

void FarmlandTile::setTileSize(float size)
{
    tileSize = size;
}

void FarmlandTile::update(float dt) {
    // 随时间减少水分
    if (moisture > 0) {
        moisture -= dt * 5.0f; // 每秒减少5%的水分
        if (moisture <= 0) {
            moisture = 0;
            if (currentState == State::WATERED) {
                currentState = State::TILLED;
                updateSprite();
            }
        }
    }
}

void FarmlandTile::updateSprite() {
    // 根据状态更新显示的精灵
    const char* spritePath = nullptr;
    switch (currentState) {
        case State::EMPTY:
            spritePath = "farmland/soil_empty.png";
            break;
        case State::TILLED:
            spritePath = "farmland/soil_tilled.png";
            break;
        case State::WATERED:
            spritePath = "farmland/soil_watered.png";
            break;
        case State::FERTILIZED:
            spritePath = "farmland/soil_fertilized.png";
            break;
    }

    if (spritePath) {
        auto sprite = Sprite::create(spritePath);
        if (sprite) {
            sprite->setContentSize(Size(40.0f, 40.0f));  // 使用基础大小
            float scale = tileSize / 32.0f;  // 计算需要的缩放比例
            sprite->setScale(scale);  // 应用缩放
            sprite->setAnchorPoint(Vec2(0, 0));
            this->removeAllChildren();
            this->addChild(sprite);
            CCLOG("Created farmland sprite with scale: %.2f", scale);
        }
    }
}