#include "WeatherEffectManager.h"
#include "RenderConstants.h"

WeatherEffectManager* WeatherEffectManager::instance = nullptr;

WeatherEffectManager::WeatherEffectManager()
    : currentScene(nullptr)
    , currentEffect(nullptr) {
}

WeatherEffectManager::~WeatherEffectManager() {
    clearCurrentEffect();
    instance = nullptr;
}

WeatherEffectManager* WeatherEffectManager::getInstance() {
    if (!instance) {
        instance = new (std::nothrow) WeatherEffectManager();
        if (instance && instance->init()) {
            instance->autorelease();
        }
        else {
            CC_SAFE_DELETE(instance);
        }
    }
    return instance;
}

bool WeatherEffectManager::init() {
    if (!Node::init()) {
        return false;
    }
    
    // 注册为天气系统的观察者
    WeatherSystem::getInstance()->addWeatherChangeListener("WeatherEffectManager",
        [this](const WeatherChangeEvent& event) {
            this->updateWeatherEffect(WeatherSystem::weatherToString(event.newWeather));
        });
    
    return true;
}

void WeatherEffectManager::initializeWithScene(Scene* scene) {
    currentScene = scene;
    if (currentScene) {
        currentScene->addChild(this);
    }
}

void WeatherEffectManager::updateWeatherEffect(const std::string& weatherType) {
    clearCurrentEffect();
    
    if (weatherType == "rainy") {
        createRainEffect();
    } else if (weatherType == "snowy") {
        createSnowEffect();
    } else if (weatherType == "foggy") {
        createFogEffect();
    }
}

void WeatherEffectManager::clearCurrentEffect() {
    if (currentEffect) {
        currentEffect->removeFromParent();
        currentEffect = nullptr;
    }
}

void WeatherEffectManager::createRainEffect() {
    if (!currentScene) return;

    auto rain = ParticleRain::create();
    if (rain) {
        rain->setVisible(true);
        rain->setPosition(Vec2(Director::getInstance()->getWinSize().width / 2,
                             Director::getInstance()->getWinSize().height + 50));
        rain->setLife(1.2f);
        rain->setLifeVar(0.4f);
        rain->setEmissionRate(800.0f);    // 增加发射率
        rain->setStartSize(14.0f);        // 增大雨滴尺寸
        rain->setStartSizeVar(6.0f);
        rain->setEndSize(8.0f);
        rain->setEndSizeVar(2.0f);
        rain->setSpeed(900.0f);
        rain->setSpeedVar(100.0f);
        // 设置为浅蓝色
        rain->setStartColor(Color4F(0.7f, 0.8f, 1.0f, 1.0f));
        rain->setEndColor(Color4F(0.7f, 0.8f, 1.0f, 0.3f));
        rain->setPosVar(Vec2(Director::getInstance()->getWinSize().width / 2, 0));
        
        currentEffect = rain;
        currentScene->addChild(currentEffect, 9999);
    }
}

void WeatherEffectManager::createSnowEffect() {
    if (!currentScene) return;

    auto snow = ParticleSnow::create();
    if (snow) {
        snow->setVisible(true);
        snow->setPosition(Vec2(Director::getInstance()->getWinSize().width / 2,
                             Director::getInstance()->getWinSize().height + 50));
        snow->setLife(4.0f);
        snow->setLifeVar(1.0f);
        snow->setEmissionRate(300.0f);    // 增加发射率
        snow->setStartSize(12.0f);        // 增大雪花尺寸
        snow->setStartSizeVar(6.0f);
        snow->setEndSize(4.0f);
        snow->setEndSizeVar(2.0f);
        snow->setSpeed(100.0f);
        snow->setSpeedVar(30.0f);
        snow->setStartColor(Color4F(1.0f, 1.0f, 1.0f, 1.0f));
        snow->setEndColor(Color4F(1.0f, 1.0f, 1.0f, 0.3f));
        snow->setPosVar(Vec2(Director::getInstance()->getWinSize().width / 2, 0));
        
        currentEffect = snow;
        currentScene->addChild(currentEffect, 9999);
    }
}

void WeatherEffectManager::createFogEffect() {
    if (!currentScene) return;

    auto fog = ParticleRain::create();
    if (fog) {
        fog->setVisible(true);
        fog->setEmitterMode(ParticleSystem::Mode::GRAVITY);
        fog->setPosition(Vec2(Director::getInstance()->getWinSize().width / 2,
                            Director::getInstance()->getWinSize().height / 2));
        fog->setLife(8.0f);
        fog->setLifeVar(2.0f);
        fog->setEmissionRate(20.0f);             // 增加发射率
        fog->setStartSize(200.0f);               // 增大雾粒子尺寸
        fog->setStartSizeVar(80.0f);
        fog->setEndSize(160.0f);
        fog->setEndSizeVar(40.0f);
        fog->setSpeed(10.0f);
        fog->setSpeedVar(5.0f);
        fog->setStartColor(Color4F(0.8f, 0.8f, 0.8f, 0.1f));
        fog->setEndColor(Color4F(0.8f, 0.8f, 0.8f, 0.0f));
        fog->setPosVar(Vec2(Director::getInstance()->getWinSize().width / 4,
                           Director::getInstance()->getWinSize().height / 4));
        
        currentEffect = fog;
        currentScene->addChild(currentEffect, 9999);
    }
}
