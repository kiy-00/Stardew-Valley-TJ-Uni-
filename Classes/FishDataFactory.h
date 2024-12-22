#ifndef __FISH_DATA_FACTORY_H__
#define __FISH_DATA_FACTORY_H__

#include <vector>
#include "FishInfo.h"

/**
 * @class FishDataFactory
 * @brief 钓鱼数据工厂类
 * 负责创建钓鱼场景中的鱼数据，用于生成多条鱼的相关信息。
 */
class FishDataFactory {
public:
    static std::vector<FishInfo> createFishData();  // 生成一个包含多条鱼信息的向量
};

#endif


