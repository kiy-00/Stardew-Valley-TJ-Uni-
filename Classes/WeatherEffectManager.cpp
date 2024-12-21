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
        snow->setEmitterMode(ParticleSystem::Mode::GRAVITY);  // 使用重力模式
        snow->setPosition(Vec2(Director::getInstance()->getWinSize().width / 2,
                             Director::getInstance()->getWinSize().height + 50));
        snow->setLife(6.0f);
        snow->setLifeVar(1.0f);
        snow->setEmissionRate(300.0f);
        snow->setStartSize(15.0f);        // 增大雪花尺寸
        snow->setStartSizeVar(8.0f);
        snow->setEndSize(12.0f);          // 调整结束尺寸
        snow->setEndSizeVar(4.0f);
        snow->setSpeed(160.0f);           // 增加下落速度
        snow->setSpeedVar(50.0f);
        snow->setGravity(Vec2(0, -100));  // 添加重力
        snow->setAngle(90);               // 设置角度
        snow->setAngleVar(10);            // 角度变化范围
        // 修改为淡蓝色，增加不透明度
        snow->setStartColor(Color4F(0.7f, 0.8f, 1.0f, 0.9f));  // 淡蓝色起始
        snow->setEndColor(Color4F(0.7f, 0.8f, 1.0f, 0.7f));    // 淡蓝色结束
        snow->setPosVar(Vec2(Director::getInstance()->getWinSize().width / 2, 0));
        
        currentEffect = snow;
        currentScene->addChild(currentEffect, 9999);
        
        // 输出调试信息
        CCLOG("Snow effect created - Emission Rate: %f, Start Size: %f", 
              snow->getEmissionRate(), snow->getStartSize());
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
