#include "CropQualityStates.h"
#include "Crop.h"
void NormalQualityState::updateQualityPoints(Crop* crop, bool isWatered, bool isFertilized, float fertility) {
    float points = crop->getQualityPoints();
    // 基础增长
    float baseIncrease = 1.0f;
    // 条件加成
    if (isWatered && isFertilized) {
        baseIncrease *= 2.0f;
    }
    else if (isWatered) {
        baseIncrease *= 1.5f;
    }
    // 土地肥力加成
    baseIncrease *= (1.0f + fertility / 100.0f);

    points += baseIncrease;
    crop->setQualityPoints(points);

    // 检查是否达到升级条件
    if (points >= 150) {
        crop->setQualityState(new SilverQualityState());
    }
}
void SilverQualityState::updateQualityPoints(Crop* crop, bool isWatered, bool isFertilized, float fertility) {
    float points = crop->getQualityPoints();
    // 银质等级基础增长率稍高
    float baseIncrease = 1.5f;

    // 条件加成
    if (isWatered && isFertilized) {
        baseIncrease *= 2.2f;  // 略高于普通品质的加成
    }
    else if (isWatered) {
        baseIncrease *= 1.7f;
    }

    // 土地肥力加成，银质等级对肥力更敏感
    baseIncrease *= (1.0f + fertility / 90.0f);

    points += baseIncrease;
    crop->setQualityPoints(points);

    // 检查是否达到黄金品质
    if (points >= 300) {
        crop->setQualityState(new GoldQualityState());
    }
}
void GoldQualityState::updateQualityPoints(Crop* crop, bool isWatered, bool isFertilized, float fertility) {
    float points = crop->getQualityPoints();
    // 黄金等级基础增长率最高
    float baseIncrease = 2.0f;

    // 条件加成
    if (isWatered && isFertilized) {
        baseIncrease *= 2.5f;  // 最高品质的最大加成
    }
    else if (isWatered) {
        baseIncrease *= 2.0f;
    }

    // 土地肥力加成，黄金等级对肥力最敏感
    baseIncrease *= (1.0f + fertility / 80.0f);

    points += baseIncrease;

    // 黄金品质有上限
    points = std::min(points, 500.0f);

    crop->setQualityPoints(points);
    // 黄金品质是最高等级，不需要检查升级
}