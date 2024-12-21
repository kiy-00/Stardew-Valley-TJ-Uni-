#pragma once
#include "cocos2d.h"
#include "Crop.h"
#include "FarmMapManager.h"
#include "FarmConfigManager.h"

USING_NS_CC;

class CropsManager {
private:
    static CropsManager* instance;
    Layer* cropsLayer;
    std::map<std::string, Crop*> crops;

    std::string currentFarmType;  // 当前农场类型
    std::string currentSeason;    // 当前季节

    CropsManager() {}
    ~CropsManager() { instance = nullptr; }

public:
    static CropsManager* getInstance();

    bool init(cocos2d::Scene* scene, const std::string& farmType, const std::string& season, int zOrder);
    void setFarmType(const std::string& farmType) { currentFarmType = farmType; }
    const std::string& getFarmType() const { return currentFarmType; }

    bool isCropAvailable(const std::string& cropType) const;
    void plantCrop(const std::string& cropType, const Vec2& worldPos);
    void harvestCrop(const Vec2& worldPos);
    void removeCrop(const Vec2& worldPos);
    Crop* getCropAt(const Vec2& worldPos);
    void handleToolAction(const std::string& toolType, const Vec2& worldPos, int direction);
    void handleWeatherChange(float temperature, const std::string& weather);
    void handleSeasonChange(const std::string& season);
    void update(float dt);

    std::vector<std::string> getAvailableCrops() const;

private:
    std::string positionToString(const Vec2& pos);
    bool checkPlantingConditions(const std::string& cropType, const Vec2& worldPos) const;
};
