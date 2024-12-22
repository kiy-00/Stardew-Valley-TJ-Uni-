#ifndef __ANIMAL_H__
#define __ANIMAL_H__

#include "cocos2d.h"
#include "AnimalConfig.h"
#include <functional>
#include <unordered_map>
#include <unordered_set>


class Animal : public cocos2d::Sprite
{
public:
	// 定义动物的移动方向（上下左右）
	enum class Direction {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	static Animal* createWithType(const std::string& typeName);    // 静态工厂方法（创建动物对象）
	bool initWithConfig(const AnimalConfig& config);               // 初始化方法（设置动物配置）
	void touchAnimal();                                            // 动物被触摸时调用
	void update(float dt) override;                                // 每帧更新时调用
	void startRandomMove();                                        // 开始随机移动
	std::string getAnimalType() const { return _config.typeName; } // 获取动物类型

	std::function<void(const std::string&, const cocos2d::Vec2&)> produceCallback;  // 回调函数类型，用于生成物品

protected:
	AnimalConfig _config;         // 动物的配置
	int _favor;                   // 动物的好感度
	bool _canProduce;             // 表示动物是否可以产生物品
	cocos2d::Action* _currentAction; // 当前执行的动作

	static std::unordered_set<std::string> _loadedPlists; // 已加载的plist文件

	void tryProduce();                      // 生产物品
	void playIdleAnimation();               // 播放动物静止动画
	void playWalkAnimation(Direction dir);  // 播放动物行走动画
	void onMoveFinished();                  // 结束行走
	void doRandomMove();                    // 执行随机移动
	Direction getDirectionForMovement(const cocos2d::Vec2& start, const cocos2d::Vec2& end); // 计算移动方向

	void loadAnimationFrames(const std::string& state, Direction direction); // 加载“动画帧”
	cocos2d::Animate* createAnimate(const std::string& framePrefix, int frameCount, float delayPerUnit = 0.2f); // 创建动画

	static const std::unordered_map<std::string, AnimalConfig>& getAnimalConfigs();  // 配置管理
};

#endif // __ANIMAL_H__
