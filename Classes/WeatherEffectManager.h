#pragma once
#include "cocos2d.h"
#include "WeatherSystem.h"

USING_NS_CC;

class WeatherEffectManager : public Node {
public:
    static WeatherEffectManager* getInstance();
    virtual bool init() override;

    // 初始化天气效果
    void initializeWithScene(Scene* scene);

    // 更新天气效果
    void updateWeatherEffect(const std::string& weatherType);

    // 清除当前天气效果
    void clearCurrentEffect();

private:
    WeatherEffectManager();
    ~WeatherEffectManager();

    // 创建雨天效果
    void createRainEffect();
    void createSnowEffect();
    void createFogEffect();

    //创建云天效果
    void createCloudEffect();
    //void clearEffect();

private:
    static WeatherEffectManager* instance;
    Scene* currentScene;
    //ParticleSystem* currentEffect;

    Node* currentEffect;

    //Layer* cloudLayer;  // 专门用于云效果

    // 禁止拷贝
    WeatherEffectManager(const WeatherEffectManager&) = delete;
    WeatherEffectManager& operator=(const WeatherEffectManager&) = delete;
};
