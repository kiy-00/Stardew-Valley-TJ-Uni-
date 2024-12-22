#ifndef __ANIMAL_CONFIG_H__
#define __ANIMAL_CONFIG_H__

#include <string>
#include "cocos2d.h"


struct AnimalConfig 
{
	std::string typeName;             // 动物类型名称，如 "chicken", "sheep"
	std::string produceItem;          // 生产物品名称，如 "egg.png", "wool.png"
	std::string idleAnimationPrefix;  // 空闲动画前缀，如 "chicken_idle_", "sheep_idle_"
	std::string walkAnimationPrefix;  // 行走动画前缀，如 "chicken_walk_", "sheep_walk_"
	int produceFavorThreshold;        // 生产物品所需好感度阈值
	int favorIncrement;               // 每次触摸增加的好感度
};

#endif // __ANIMAL_CONFIG_H__

