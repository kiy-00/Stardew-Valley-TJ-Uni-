#include "TimeEffectManager.h"
#include "RenderConstants.h"

// 初始化静态成员
TimeEffectManager* TimeEffectManager::instance = nullptr;

// 初始化颜色常量
const Color3B TimeEffectManager::EffectParams::DAWN_COLOR = Color3B(150, 170, 200);
const Color3B TimeEffectManager::EffectParams::DUSK_COLOR = Color3B(255, 170, 120);
const Color3B TimeEffectManager::EffectParams::NIGHT_COLOR = Color3B(20, 30, 60);

TimeEffectManager::TimeEffectManager()
    : currentScene(nullptr)
    , currentEffect(nullptr) {
}

TimeEffectManager::~TimeEffectManager() {
    clearCurrentEffect();
    instance = nullptr;
}

TimeEffectManager* TimeEffectManager::getInstance() {
    if (!instance) {
        instance = new (std::nothrow) TimeEffectManager();
        if (instance && instance->init()) {
            instance->autorelease();
        }
        else {
            CC_SAFE_DELETE(instance);
        }
    }
    return instance;
}

bool TimeEffectManager::init() {
    if (!Node::init()) {
        return false;
    }
    return true;
}

void TimeEffectManager::initializeWithScene(Scene* scene) {
    currentScene = scene;

    // 立即执行一次更新
    auto timeSystem = TimeSeasonSystem::getInstance();
    if (timeSystem) {
        auto currentTime = timeSystem->getCurrentTime();
        this->updateTimeEffect(currentTime.hour);
    }
}

void TimeEffectManager::clearCurrentEffect() {
    if (currentEffect) {
        currentEffect->removeFromParent();
        currentEffect = nullptr;
    }
}

void TimeEffectManager::updateTimeEffect(int hour) {
    clearCurrentEffect();

    CCLOG("Updating time effect for hour: %d", hour);

    if (hour >= TimeRanges::DAWN_START && hour < TimeRanges::DAWN_END) {
        createDawnEffect();
    }
    else if (hour >= TimeRanges::DAY_START && hour < TimeRanges::DAY_END) {
        createDayEffect();
    }
    else if (hour >= TimeRanges::DUSK_START && hour < TimeRanges::DUSK_END) {
        createDuskEffect();
    }
    else {
        createNightEffect();
    }
}

void TimeEffectManager::createDayEffect() {
    if (!currentScene) return;
    clearCurrentEffect();  // 白天不需要效果
}

void TimeEffectManager::createDawnEffect() {
    if (!currentScene) return;

    currentEffect = LayerColor::create(Color4B(EffectParams::DAWN_COLOR, EffectParams::DAWN_ALPHA));

    // 使用更大的尺寸和居中定位
    Size effectSize(3200, 3200);
    currentEffect->setContentSize(effectSize);

    // 计算居中位置
    Size mapSize(3200, 3200);
    currentEffect->setPosition(Vec2(
        -effectSize.width / 2 + mapSize.width / 2,
        -effectSize.height / 2 + mapSize.height / 2
    ));

    currentScene->addChild(currentEffect, FIFTH);

    BlendFunc blend = { GL_SRC_ALPHA, GL_ONE };
    currentEffect->setBlendFunc(blend);

    auto fadeIn = FadeTo::create(2.0f, EffectParams::DAWN_ALPHA_MIN);
    auto fadeOut = FadeTo::create(2.0f, EffectParams::DAWN_ALPHA);
    auto sequence = Sequence::create(fadeIn, fadeOut, nullptr);
    currentEffect->runAction(RepeatForever::create(sequence));
}

void TimeEffectManager::createDuskEffect() {
    if (!currentScene) return;

    currentEffect = LayerColor::create(Color4B(EffectParams::DUSK_COLOR, EffectParams::DUSK_ALPHA));

    // 使用更大的尺寸和居中定位
    Size effectSize(3200, 3200);
    currentEffect->setContentSize(effectSize);

    // 计算居中位置
    Size mapSize(3200, 3200);
    currentEffect->setPosition(Vec2(
        -effectSize.width / 2 + mapSize.width / 2,
        -effectSize.height / 2 + mapSize.height / 2
    ));

    currentScene->addChild(currentEffect, FIFTH);

    BlendFunc blend = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
    currentEffect->setBlendFunc(blend);

    auto fadeIn = FadeTo::create(3.0f, EffectParams::DUSK_ALPHA_MAX);
    auto fadeOut = FadeTo::create(3.0f, EffectParams::DUSK_ALPHA_MIN);
    auto sequence = Sequence::create(fadeIn, fadeOut, nullptr);
    currentEffect->runAction(RepeatForever::create(sequence));
}

void TimeEffectManager::createNightEffect() {
    if (!currentScene) return;

    currentEffect = LayerColor::create(Color4B(EffectParams::NIGHT_COLOR, EffectParams::NIGHT_ALPHA));

    // 使用更大的尺寸和居中定位
    Size effectSize(3200, 3200);
    currentEffect->setContentSize(effectSize);

    // 计算居中位置
    Size mapSize(3200, 3200);
    currentEffect->setPosition(Vec2(
        -effectSize.width / 2 + mapSize.width / 2,
        -effectSize.height / 2 + mapSize.height / 2
    ));

    currentScene->addChild(currentEffect, FIFTH);

    BlendFunc blend = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };
    currentEffect->setBlendFunc(blend);

    auto fadeIn = FadeTo::create(4.0f, EffectParams::NIGHT_ALPHA_MAX);
    auto fadeOut = FadeTo::create(4.0f, EffectParams::NIGHT_ALPHA_MIN);
    auto sequence = Sequence::create(fadeIn, fadeOut, nullptr);
    currentEffect->runAction(RepeatForever::create(sequence));
}