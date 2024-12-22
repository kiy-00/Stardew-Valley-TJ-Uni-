#include "Animal.h"
#include "cocos2d.h"
#include <cstdlib>

USING_NS_CC;

// 初始化静态成员
std::unordered_set<std::string> Animal::_loadedPlists;

/**
 * @brief 获取所有动物配置
 */
const std::unordered_map<std::string, AnimalConfig>& Animal::getAnimalConfigs()
{
    static std::unordered_map<std::string, AnimalConfig> configs = {
        {"chicken", {"chicken", "egg.png", "chicken_idle_", "chicken_walk_", 45, 15}},
        {"sheep", {"sheep", "wool.png", "sheep_idle_", "sheep_walk_", 60, 15}},
        // 在这里添加更多动物类型的配置
    };
    return configs;
}

/**
 * @brief 创建指定类型的Animal对象
 */
Animal* Animal::createWithType(const std::string& typeName)
{
    auto it = getAnimalConfigs().find(typeName);
    if (it == getAnimalConfigs().end()) {
        CCLOG("Unknown animal type: %s", typeName.c_str());
        return nullptr;
    }

    Animal* animal = new (std::nothrow) Animal();
    if (animal && animal->initWithConfig(it->second))
    {
        animal->autorelease();
        return animal;
    }
    CC_SAFE_DELETE(animal);
    return nullptr;
}

/**
 * @brief 根据动物配置初始化对象
 */
bool Animal::initWithConfig(const AnimalConfig& config)
{
    if (!Sprite::init())
        return false;

    _config = config;
    _favor = 0;
    _canProduce = false;
    _currentAction = nullptr;

    // 设置初始动画及行为
    playIdleAnimation();
    scheduleUpdate();        // 注册更新方法
    startRandomMove();       // 开始随机移动

    return true;
}

/**
 * @brief 玩家触摸动物时调用，增加好感度
 */
void Animal::touchAnimal()
{
    _favor += _config.favorIncrement; // 根据配置增加好感度
    if (_favor >= _config.produceFavorThreshold) {
        _canProduce = true;  // 达到阈值则允许生产
    }
}

/**
 * @brief 每帧更新逻辑，检测是否可以生产
 */
void Animal::update(float dt)
{
    if (_canProduce) {
        tryProduce();
    }
}

/**
 * @brief 尝试生产物品
 */
void Animal::tryProduce() {
	if (_favor >= _config.produceFavorThreshold) {
		if (produceCallback) {
			produceCallback(_config.produceItem, getPosition());
		}
		_favor = 0;          // 重置好感度
		_canProduce = false; // 关闭生产状态
		CCLOG("%s produced!", _config.produceItem.c_str());
	}
}


/**
 * @brief 播放空闲状态动画
 */
void Animal::playIdleAnimation()
{
    loadAnimationFrames("idle", Direction::DOWN); // 默认朝向下
    std::string prefix = _config.idleAnimationPrefix + "down_";

    auto animate = createAnimate(prefix, 2, 0.5f); // 生成动画
    this->stopAllActions();
    this->runAction(RepeatForever::create(animate)); // 循环播放动画
}

/**
 * @brief 播放移动状态动画
 * @param dir 移动方向
 */
void Animal::playWalkAnimation(Direction dir)
{
    loadAnimationFrames("walk", dir);
    std::string prefix = _config.walkAnimationPrefix;

    switch (dir) {  // 根据方向选择帧前缀
    case Direction::UP: prefix += "up_"; break;
    case Direction::DOWN: prefix += "down_"; break;
    case Direction::LEFT: prefix += "left_"; break;
    case Direction::RIGHT: prefix += "right_"; break;
    }

    auto animate = createAnimate(prefix, 4, 0.2f); // 生成动画
    this->stopAllActions();
    this->runAction(RepeatForever::create(animate)); // 循环播放动画
}

/**
 * @brief 开始随机移动逻辑
 */
void Animal::startRandomMove()
{
    float delayTime = RandomHelper::random_real(3.0f, 6.0f); // 随机延时
    this->runAction(Sequence::create(
        DelayTime::create(delayTime),
        CallFunc::create([this]() {
            this->doRandomMove();
            }),
        nullptr));
}

/**
 * @brief 执行一次随机移动
 */
void Animal::doRandomMove()
{
	if (!this->getParent()) return;
	auto parentSize = this->getParent()->getContentSize(); // 获取父节点大小
	Vec2 startPos = getPosition(); // 当前动物位置

	// 调整随机生成目标位置的范围（减少到原来的1/10）
	Vec2 targetPos(
		RandomHelper::random_real(startPos.x - 10.0f, startPos.x + 10.0f),
		RandomHelper::random_real(startPos.y - 10.0f, startPos.y + 10.0f)
	);

	// 确保目标位置在父节点范围内
	targetPos.x = clampf(targetPos.x, 50.0f, parentSize.width - 50.0f);
	targetPos.y = clampf(targetPos.y, 50.0f, parentSize.height - 50.0f);

	Direction moveDir = getDirectionForMovement(startPos, targetPos); // 计算移动方向
	playWalkAnimation(moveDir);

	// 调整移动持续时间（减少到原来的1/10）
	float moveDuration = 0.2f;

	auto moveTo = MoveTo::create(moveDuration, targetPos);
	auto seq = Sequence::create(moveTo, CallFunc::create([this]() {
		this->onMoveFinished();
		}), nullptr);
	this->runAction(seq); // 执行动画序列
}

/**
 * @brief 随机移动结束后的回调
 */
void Animal::onMoveFinished()
{
    playIdleAnimation();
    startRandomMove(); // 继续下一轮随机移动
}

/**
 * @brief 根据帧前缀生成动画
 * @param framePrefix 帧前缀
 * @param frameCount 帧数
 * @param delayPerUnit 每帧延迟时间
 * @return 生成的Animate对象
 */
Animate* Animal::createAnimate(const std::string& framePrefix, int frameCount, float delayPerUnit)
{
    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= frameCount; i++) {
        char frameName[60];
        sprintf(frameName, "%s%02d.png", framePrefix.c_str(), i);
        if (!framePrefix.empty()) frameName[0] = toupper(frameName[0]); // 帧名称首字母转大写
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame) {
            frames.pushBack(frame);
        }
        else {
            CCLOG("Frame not found: %s", frameName);
        }
    }

    auto animation = Animation::createWithSpriteFrames(frames, delayPerUnit);
    return Animate::create(animation);
}

/**
 * @brief 加载指定状态和方向的动画帧
 * @param state 动物状态（空闲或移动）
 * @param direction 动物朝向
 */
void Animal::loadAnimationFrames(const std::string& state, Direction direction)
{
    std::string directionStr;
    switch (direction) {
    case Direction::UP: directionStr = "up"; break;
    case Direction::DOWN: directionStr = "down"; break;
    case Direction::LEFT: directionStr = "left"; break;
    case Direction::RIGHT: directionStr = "right"; break;
    }

    std::string animalTypeStr = _config.typeName;
    std::string stateStr = state;
    std::string plistPath = "animals/" + animalTypeStr + "_" + stateStr + "_" + directionStr + ".plist";

    // 只加载未加载过的plist
    if (_loadedPlists.find(plistPath) == _loadedPlists.end()) {
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath);
        _loadedPlists.insert(plistPath);
    }
}

/**
 * @brief 根据起点和终点计算移动方向
 * @param start 起点坐标
 * @param end 终点坐标
 * @return 移动方向
 */
Animal::Direction Animal::getDirectionForMovement(const Vec2& start, const Vec2& end)
{
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    if (fabs(dx) > fabs(dy)) {
        return (dx > 0) ? Direction::RIGHT : Direction::LEFT;
    }
    else {
        return (dy > 0) ? Direction::UP : Direction::DOWN;
    }
}
