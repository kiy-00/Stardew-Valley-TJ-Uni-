#include "TimeSeasonSystem.h"
#include "WeatherSystem.h"
#include "FarmlandTile.h"

//// 季节枚举（应该在 TimeSeasonSystem.h 中定义）
//enum class Season {
//    SPRING,
//    SUMMER,
//    FALL,
//    WINTER
//};
//
//// 季节变化事件结构（应该在 TimeSeasonSystem.h 中定义）
//struct SeasonChangeEvent {
//    Season previousSeason;
//    Season newSeason;
//    int year;
//    int day;
//};

class Crop : public Node {
protected:
    enum class GrowthStage {
        SEED,
        STAGE1,
        STAGE2,
        STAGE3,
        DEAD
    };

    float growthProgress = 0.0f;  // 生长进度(0-100)
    GrowthStage currentStage = GrowthStage::SEED;
    bool isWatered = false;
    bool isFertilized = false;
    int deathCounter = 0;  // 用于追踪不良环境持续时间

    FarmlandTile* farmland = nullptr;
    std::string cropType;  // "berry", "carrot", "pepper", "wheat"

    // 环境影响系数
    float seasonGrowthModifier = 1.0f;
    float weatherGrowthModifier = 1.0f;

    // 作物特性(子类可重写)
    struct CropProperties {
        float growthRate = 1.0f;        // 基础生长速度
        float waterNeed = 50.0f;        // 需水量
        float fertilityNeed = 30.0f;    // 需肥量
        std::vector<float> stageThresholds = { 25.0f, 50.0f, 75.0f, 100.0f };  // 生长阶段阈值

        // 不同天气的生长系数
        std::map<std::string, float> weatherModifiers = {
            {"sunny", 1.2f},
            {"cloudy", 1.0f},
            {"rainy", 1.1f},
            {"snowy", 0.5f},
            {"foggy", 0.8f}
        };

        // 不同季节的生长系数
        std::map<std::string, float> seasonModifiers = {
            {"spring", 1.2f},
            {"summer", 1.0f},
            {"fall", 0.8f},
            {"winter", 0.5f}
        };
    } properties;

public:
    static Crop* create(const std::string& type, FarmlandTile* tile);
    virtual bool init(const std::string& type, FarmlandTile* tile);

    void update(float dt) override;

    bool canHarvest() const;
    virtual void harvest();

protected:
    void updateSprite();
    virtual float calculateGrowthRate();
    void checkGrowthStage();
    bool checkEnvironmentConditions();

    void setupEventListeners();
    void removeEventListeners();

    virtual ~Crop() {
        removeEventListeners();
    }
};

