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
    return true;
}

void WeatherEffectManager::initializeWithScene(Scene* scene) {
    currentScene = scene;

    // 注册为天气系统的观察者
    WeatherSystem::getInstance()->addWeatherChangeListener("WeatherEffectManager",
        [this](const WeatherChangeEvent& event) {
            this->updateWeatherEffect(WeatherSystem::weatherToString(event.newWeather));
        });
}

void WeatherEffectManager::updateWeatherEffect(const std::string& weatherType) {
    // 先清除当前特效
    clearCurrentEffect();
    
    CCLOG("Creating weather effect for: %s", weatherType.c_str());
    
    if (weatherType == "rainy") {
        createRainEffect();
        CCLOG("Rain effect created");
    }
    else if (weatherType == "snowy") {
        createSnowEffect();
        CCLOG("Snow effect created");
    }
    else if (weatherType == "foggy") {
        createFogEffect();
        CCLOG("Fog effect created");
    }
    else if (weatherType == "cloudy") {
        createCloudEffect();
        CCLOG("Cloud effect created");
    }
    else {
        CCLOG("No effect created for weather type: %s", weatherType.c_str());
    }
}

void WeatherEffectManager::clearCurrentEffect() {
    if (currentEffect) {
        CCLOG("Clearing current weather effect");
        currentEffect->removeFromParent();
        currentEffect = nullptr;
    }
}

void WeatherEffectManager::createRainEffect() {
    if (!currentScene) {
        CCLOG("Error: No current scene set for rain effect");
        return;
    }

    auto rain = ParticleRain::create();
    if (rain) {
        // 设置发射源在屏幕顶端中间，并略高于可视范围
        rain->setPosition(Vec2(
            Director::getInstance()->getWinSize().width / 2,
            Director::getInstance()->getWinSize().height + 50)
        );

        // 调整雨滴排布范围，确保雨能覆盖全屏宽度
        rain->setPosVar(Vec2(
            Director::getInstance()->getWinSize().width / 2,
            0.0f)
        );

        // 控制每秒发射的雨滴数量
        rain->setEmissionRate(800.0f);

        // 雨滴存活时间，设置相对短一些，让雨滴尽快落到屏幕底端消失
        // 可根据屏幕高度和速度来决定
        rain->setLife(1.2f);
        rain->setLifeVar(0.4f);

        // 雨滴的大小
        // 对比雪花，雨滴通常更小、更细
        rain->setStartSize(14.0f);
        rain->setStartSizeVar(6.0f);
        rain->setEndSize(8.0f);
        rain->setEndSizeVar(2.0f);

        // 雨滴下落速度要比雪大得多
        // 这里 speed=900，speedVar=100，表示平均速度900像素/秒，随机波动±100
        rain->setSpeed(900.0f);
        rain->setSpeedVar(100.0f);

        // 雨滴的颜色，偏蓝 + 一些透明度
        // startColor = 雨滴初始颜色
        // endColor   = 雨滴将要消失时的颜色
        rain->setStartColor(Color4F(0.7f, 0.8f, 1.0f, 1.0f));
        rain->setEndColor(Color4F(0.7f, 0.8f, 1.0f, 0.3f));

        // 可根据需要修改重力方向（默认为向下）
        // rain->setGravity(Vec2(0, -1000)); // 如果要显式指定重力，可加这行

        currentEffect = rain;
        currentScene->addChild(currentEffect, 9999);
        CCLOG("Rain effect parameters set and added to scene");
    }
    else {
        CCLOG("Error: Failed to create rain particle system");
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

void WeatherEffectManager::createCloudEffect() {
    if (!currentScene) return;

    // 创建云层
    auto cloudLayer = Layer::create();
    currentScene->addChild(cloudLayer, 9998);
    currentEffect = cloudLayer;

    // 在四个角附近立即生成云朵
    const std::vector<Vec2> initialPositions = {
        // 左上角区域
        Vec2(Director::getInstance()->getWinSize().width * 0.1f,
             Director::getInstance()->getWinSize().height * 0.9f),
        Vec2(Director::getInstance()->getWinSize().width * 0.2f,
             Director::getInstance()->getWinSize().height * 0.85f),
        Vec2(Director::getInstance()->getWinSize().width * 0.15f,
             Director::getInstance()->getWinSize().height * 0.8f),
        Vec2(Director::getInstance()->getWinSize().width * 0.25f,
             Director::getInstance()->getWinSize().height * 0.75f),

             // 右上角区域
             Vec2(Director::getInstance()->getWinSize().width * 0.9f,
                  Director::getInstance()->getWinSize().height * 0.9f),
             Vec2(Director::getInstance()->getWinSize().width * 0.8f,
                  Director::getInstance()->getWinSize().height * 0.85f),
             Vec2(Director::getInstance()->getWinSize().width * 0.85f,
                  Director::getInstance()->getWinSize().height * 0.8f),
             Vec2(Director::getInstance()->getWinSize().width * 0.75f,
                  Director::getInstance()->getWinSize().height * 0.75f),

                  // 左下角区域
                  Vec2(Director::getInstance()->getWinSize().width * 0.1f,
                       Director::getInstance()->getWinSize().height * 0.3f),
                  Vec2(Director::getInstance()->getWinSize().width * 0.2f,
                       Director::getInstance()->getWinSize().height * 0.35f),
                  Vec2(Director::getInstance()->getWinSize().width * 0.15f,
                       Director::getInstance()->getWinSize().height * 0.4f),
                  Vec2(Director::getInstance()->getWinSize().width * 0.25f,
                       Director::getInstance()->getWinSize().height * 0.45f),

                       // 右下角区域
                       Vec2(Director::getInstance()->getWinSize().width * 0.9f,
                            Director::getInstance()->getWinSize().height * 0.3f),
                       Vec2(Director::getInstance()->getWinSize().width * 0.8f,
                            Director::getInstance()->getWinSize().height * 0.35f),
                       Vec2(Director::getInstance()->getWinSize().width * 0.85f,
                            Director::getInstance()->getWinSize().height * 0.4f),
                       Vec2(Director::getInstance()->getWinSize().width * 0.75f,
                            Director::getInstance()->getWinSize().height * 0.45f),

                            // 上方中间区域（较稀疏）
                            Vec2(Director::getInstance()->getWinSize().width * 0.4f,
                                 Director::getInstance()->getWinSize().height * 0.8f),
                            Vec2(Director::getInstance()->getWinSize().width * 0.6f,
                                 Director::getInstance()->getWinSize().height * 0.8f),

                                 // 下方中间区域（较稀疏）
                                 Vec2(Director::getInstance()->getWinSize().width * 0.4f,
                                      Director::getInstance()->getWinSize().height * 0.4f),
                                 Vec2(Director::getInstance()->getWinSize().width * 0.6f,
                                      Director::getInstance()->getWinSize().height * 0.4f),

                                      // 左侧中间
                                      Vec2(Director::getInstance()->getWinSize().width * 0.15f,
                                           Director::getInstance()->getWinSize().height * 0.6f),
                                      Vec2(Director::getInstance()->getWinSize().width * 0.2f,
                                           Director::getInstance()->getWinSize().height * 0.55f),

                                           // 右侧中间
                                           Vec2(Director::getInstance()->getWinSize().width * 0.85f,
                                                Director::getInstance()->getWinSize().height * 0.6f),
                                           Vec2(Director::getInstance()->getWinSize().width * 0.8f,
                                                Director::getInstance()->getWinSize().height * 0.55f),
                                                // 左上角外围追加
                                                Vec2(Director::getInstance()->getWinSize().width * (0.05f + random(-0.02f, 0.02f)),
                                                     Director::getInstance()->getWinSize().height * (0.95f + random(-0.02f, 0.02f))),
                                                Vec2(Director::getInstance()->getWinSize().width * (0.08f + random(-0.02f, 0.02f)),
                                                     Director::getInstance()->getWinSize().height * (0.92f + random(-0.02f, 0.02f))),
                                                Vec2(Director::getInstance()->getWinSize().width * (0.12f + random(-0.02f, 0.02f)),
                                                     Director::getInstance()->getWinSize().height * (0.88f + random(-0.02f, 0.02f))),
                                                Vec2(Director::getInstance()->getWinSize().width * (0.18f + random(-0.02f, 0.02f)),
                                                     Director::getInstance()->getWinSize().height * (0.93f + random(-0.02f, 0.02f))),
                                                Vec2(Director::getInstance()->getWinSize().width * (0.22f + random(-0.02f, 0.02f)),
                                                     Director::getInstance()->getWinSize().height * (0.89f + random(-0.02f, 0.02f))),

                                                     // 右上角外围追加
                                                     Vec2(Director::getInstance()->getWinSize().width * (0.95f + random(-0.02f, 0.02f)),
                                                          Director::getInstance()->getWinSize().height * (0.95f + random(-0.02f, 0.02f))),
                                                     Vec2(Director::getInstance()->getWinSize().width * (0.92f + random(-0.02f, 0.02f)),
                                                          Director::getInstance()->getWinSize().height * (0.92f + random(-0.02f, 0.02f))),
                                                     Vec2(Director::getInstance()->getWinSize().width * (0.88f + random(-0.02f, 0.02f)),
                                                          Director::getInstance()->getWinSize().height * (0.88f + random(-0.02f, 0.02f))),
                                                     Vec2(Director::getInstance()->getWinSize().width * (0.82f + random(-0.02f, 0.02f)),
                                                          Director::getInstance()->getWinSize().height * (0.93f + random(-0.02f, 0.02f))),
                                                     Vec2(Director::getInstance()->getWinSize().width * (0.78f + random(-0.02f, 0.02f)),
                                                          Director::getInstance()->getWinSize().height * (0.89f + random(-0.02f, 0.02f))),

                                                          // 左下角外围追加
                                                          Vec2(Director::getInstance()->getWinSize().width * (0.05f + random(-0.02f, 0.02f)),
                                                               Director::getInstance()->getWinSize().height * (0.25f + random(-0.02f, 0.02f))),
                                                          Vec2(Director::getInstance()->getWinSize().width * (0.08f + random(-0.02f, 0.02f)),
                                                               Director::getInstance()->getWinSize().height * (0.28f + random(-0.02f, 0.02f))),
                                                          Vec2(Director::getInstance()->getWinSize().width * (0.12f + random(-0.02f, 0.02f)),
                                                               Director::getInstance()->getWinSize().height * (0.32f + random(-0.02f, 0.02f))),
                                                          Vec2(Director::getInstance()->getWinSize().width * (0.18f + random(-0.02f, 0.02f)),
                                                               Director::getInstance()->getWinSize().height * (0.27f + random(-0.02f, 0.02f))),
                                                          Vec2(Director::getInstance()->getWinSize().width * (0.22f + random(-0.02f, 0.02f)),
                                                               Director::getInstance()->getWinSize().height * (0.31f + random(-0.02f, 0.02f))),

                                                               // 右下角外围追加
                                                               Vec2(Director::getInstance()->getWinSize().width * (0.95f + random(-0.02f, 0.02f)),
                                                                    Director::getInstance()->getWinSize().height * (0.25f + random(-0.02f, 0.02f))),
                                                               Vec2(Director::getInstance()->getWinSize().width * (0.92f + random(-0.02f, 0.02f)),
                                                                    Director::getInstance()->getWinSize().height * (0.28f + random(-0.02f, 0.02f))),
                                                               Vec2(Director::getInstance()->getWinSize().width * (0.88f + random(-0.02f, 0.02f)),
                                                                    Director::getInstance()->getWinSize().height * (0.32f + random(-0.02f, 0.02f))),
                                                               Vec2(Director::getInstance()->getWinSize().width * (0.82f + random(-0.02f, 0.02f)),
                                                                    Director::getInstance()->getWinSize().height * (0.27f + random(-0.02f, 0.02f))),
                                                               Vec2(Director::getInstance()->getWinSize().width * (0.78f + random(-0.02f, 0.02f)),
                                                                    Director::getInstance()->getWinSize().height * (0.31f + random(-0.02f, 0.02f))),

                                                                    // 上边缘额外追加
                                                                    Vec2(Director::getInstance()->getWinSize().width * (0.3f + random(-0.02f, 0.02f)),
                                                                         Director::getInstance()->getWinSize().height * (0.95f + random(-0.02f, 0.02f))),
                                                                    Vec2(Director::getInstance()->getWinSize().width * (0.5f + random(-0.02f, 0.02f)),
                                                                         Director::getInstance()->getWinSize().height * (0.92f + random(-0.02f, 0.02f))),
                                                                    Vec2(Director::getInstance()->getWinSize().width * (0.7f + random(-0.02f, 0.02f)),
                                                                         Director::getInstance()->getWinSize().height * (0.95f + random(-0.02f, 0.02f))),

                                                                         // 下边缘额外追加
                                                                         Vec2(Director::getInstance()->getWinSize().width * (0.3f + random(-0.02f, 0.02f)),
                                                                              Director::getInstance()->getWinSize().height * (0.25f + random(-0.02f, 0.02f))),
                                                                         Vec2(Director::getInstance()->getWinSize().width * (0.5f + random(-0.02f, 0.02f)),
                                                                              Director::getInstance()->getWinSize().height * (0.28f + random(-0.02f, 0.02f))),
                                                                         Vec2(Director::getInstance()->getWinSize().width * (0.7f + random(-0.02f, 0.02f)),
                                                                              Director::getInstance()->getWinSize().height * (0.25f + random(-0.02f, 0.02f)))
    };

    // 立即生成初始云朵
    for (const auto& pos : initialPositions) {
        auto cloud = Sprite::create("weather/cloud.png");
        if (cloud) {
            cloud->setPosition(pos);
            cloud->setOpacity(180);
            cloud->setScale(random(0.8f, 1.2f));
            cloudLayer->addChild(cloud);

            // 给初始云朵添加轻微的漂浮动作
            float duration = random(4.0f, 6.0f);
            float offsetY = random(-20.0f, 20.0f);
            auto moveUp = MoveBy::create(duration / 2, Vec2(0, offsetY));
            auto moveDown = moveUp->reverse();
            cloud->runAction(RepeatForever::create(Sequence::create(moveUp, moveDown, nullptr)));
        }
    }

    // 定期生成新的云朵
    cloudLayer->schedule([this, cloudLayer](float dt) {
        auto cloud = Sprite::create("weather/cloud.png");
        if (cloud) {
            float startY = random(Director::getInstance()->getWinSize().height * 0.3f,
                Director::getInstance()->getWinSize().height * 0.8f);
            cloud->setPosition(Vec2(-cloud->getContentSize().width, startY));
            cloud->setOpacity(180);
            cloud->setScale(random(0.8f, 1.2f));
            cloudLayer->addChild(cloud);

            float speed = random(30.0f, 50.0f);
            float distance = Director::getInstance()->getWinSize().width +
                cloud->getContentSize().width * 2;
            float duration = distance / speed;

            auto moveAction = MoveTo::create(duration,
                Vec2(Director::getInstance()->getWinSize().width + cloud->getContentSize().width,
                    startY + random(-30.0f, 30.0f)));
            auto removeAction = RemoveSelf::create();
            cloud->runAction(Sequence::create(moveAction, removeAction, nullptr));
        }
        }, 3.0f, "create_cloud");
}