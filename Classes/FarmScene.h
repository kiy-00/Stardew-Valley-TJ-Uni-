#pragma once
#include "FarmMapManager.h"
#include "FarmInteractionManager.h"
#include "TimeSeasonSystem.h"
#include "TimeEffectManager.h"
#include "CropManager.h"
#include "cocos2d.h"
#include "User.h"
#include "Inventory.h"
#include "FarmlandManager.h"
#include "WeatherSystem.h"
#include "WeatherEffectManager.h"
#include "RenderConstants.h"
#include "AnimalFactory.h"


class FarmScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene(const std::string& mapType);
    bool init(const std::string& mapType);

    static FarmScene* create(const std::string& mapType) {
        FarmScene* scene = new (std::nothrow) FarmScene();
        if (scene && scene->init(mapType)) {
            scene->autorelease();
            return scene;
        }
        CC_SAFE_DELETE(scene);
        return nullptr;
    }

    cocos2d::TMXTiledMap* getMap()const {
        return this->tmxMap;
    }

  

private:
    // 地图相关
    cocos2d::TMXTiledMap* tmxMap;
    FarmMapManager* farmMapManager;
    std::string currentMapType;
    User* player;

    // 交互管理器
    FarmInteractionManager* interactionManager;

    // 时间季节系统相关
    TimeSeasonSystem* timeSystem;
    TimeEffectManager* timeEffectManager;
    void updateTimeSeasonLabel();     // 更新时间显示
    void onSeasonChanged(const std::string& newSeason);

    //天气系统相关
    WeatherSystem* weatherSystem;
    WeatherEffectManager* weatherEffectManager;
    Label* weatherLabel;  // For displaying current weather

    void updateWeatherLabel();
    void onWeatherChanged(const std::string& newWeather);

    // 初始化函数
    bool initMap();
    bool initPlayer();
    bool initTimeSystem();
    bool initWeatherSystem();
    void setupSystemCallbacks();
    bool createSystemLabels();
    void setupSystemSchedulers();
    void startSystems();

    void setupKeyboard();
    void initInventory();
    void setupMouse();

    // 事件处理
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onMouseClick(cocos2d::EventMouse* event);

    /* -------养殖系统------ */
    void createAnimal(const std::string& type, const Vec2& playerPos);
    void interactWithAnimals(const Vec2& playerPos, float interactionRadius);
    void pickupNearbyItems(const Vec2& playerPos, float pickupRadius);
    void generateItem(const std::string& itemName, const Vec2& position);

    // 更新函数
    void update(float dt) override;

    // 背包系统相关
    bool isInventoryInitialized = false;

    FarmlandManager* farmlandManager;
    CropManager* cropManager;
    void initFarmland();  // 新增初始化耕地的方法
    std::vector<Vec2> getFarmablePositions(); // 获取可耕种位置
};