#pragma once
#include "cocos2d.h"

USING_NS_CC;

enum class CropStage {
    SEED = 0,
    GROWING_1,
    GROWING_2,
    GROWING_3,
    MATURE,
    DEAD
};

class Crop : public Node {
public:
    static Crop* create(const std::string& cropType);
    virtual bool init() override;

protected:
    std::string cropType;
    CropStage currentStage;
    float growthTime;
    float totalGrowthTime;
    bool isWatered;
    bool isFertilized;
    Sprite* cropSprite;

    virtual void updateGrowth(float dt);
    virtual void updateSprite();
    virtual bool checkGrowthConditions();

    // 生长所需的环境条件
    struct GrowthConditions {
        float minTemp;
        float maxTemp;
        std::string preferredSeason;
        float waterNeed;
    } growthConditions;

public:
    void water() { isWatered = true; }
    void fertilize() { isFertilized = true; }
    CropStage getStage() const { return currentStage; }
    bool isFullyGrown() const { return currentStage == CropStage::MATURE; }
    virtual void handleWeatherChange(float temperature, const std::string& weather);
    virtual void handleSeasonChange(const std::string& season);
};
