#pragma once
#include "FarmMapManager.h"
#include "FarmInteractionManager.h"
#include "cocos2d.h"
#include "User.h"
#include "Inventory.h"
#include "ToolItem.h"

class FarmScene : public cocos2d::Scene {
public:
    // 修改创建场景的静态函数以接受地图类型参数
    static cocos2d::Scene* createScene(const std::string& mapType);

    // 添加初始化函数以接受地图类型参数
    bool init(const std::string& mapType);

    // 创建场景的静态方法
    static FarmScene* create(const std::string& mapType) {
        FarmScene* scene = new (std::nothrow) FarmScene();
        if (scene && scene->init(mapType)) {
            scene->autorelease();
            return scene;
        }
        CC_SAFE_DELETE(scene);
        return nullptr;
    }

private:
    // 地图相关
    cocos2d::TMXTiledMap* tmxMap;
    FarmMapManager* farmMapManager;
    std::string currentMapType;
    std::string currentSeason;
    User* player;

    // 初始化函数
    bool initMap();
    bool initPlayer();
    void setupTestMenu();
    void setupKeyboard();
    void initInventory();
    void setupMouse();

    // 事件处理
    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onMouseClick(cocos2d::EventMouse* event);

    // 更新函数
    void update(float dt) override;

    // 季节和天气相关
    void onSeasonChanged(const std::string& newSeason);
    cocos2d::Label* seasonLabel;
    const std::vector<std::string> seasons{ "spring", "summer", "fall", "winter" };
    int currentSeasonIndex;
    void setupSeasonTest();
    void switchToNextSeason(float dt);
    void updateSeasonLabel(const std::string& season);

    // 天气系统
    FarmInteractionManager* interactionManager;
    void setupWeatherTest();
    void switchToNextWeather(float dt);
    cocos2d::Label* weatherLabel;
    int currentWeatherIndex;
    std::vector<std::string> weathers = { "sunny", "cloudy", "rainy" };
    float currentGameTime = 6.0f;

    // 背包系统相关
    bool isInventoryInitialized = false;
};