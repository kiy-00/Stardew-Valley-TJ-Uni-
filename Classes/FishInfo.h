#ifndef __FISH_INFO_H__
#define __FISH_INFO_H__

#include <string>

/**
 * @struct FishInfo
 * @brief 存储鱼的信息，包括季节、天气、名称、图标等。
 */
struct FishInfo {
    std::string season;       // 季节: 春季、夏季、秋季、冬季
    std::string weather;      // 天气: 晴天、雨天
    std::string name;         // 鱼的名称
    std::string spriteName;   // 鱼的图标路径
    int gold;                 // 金币值
    int hpRecover;            // 回复生命值
    int energyRecover;        // 回复能量值
};

#endif // __FISH_INFO_H__

