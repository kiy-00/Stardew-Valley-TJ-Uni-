#pragma once
#ifndef __I_CROP_QUALITY_STATE_H__
#define __I_CROP_QUALITY_STATE_H__

#include"Enums.h"

class Crop;
class ICropQualityState {
public:
    virtual CropQuality getQuality() const = 0;
    // 新增：更新品质点数的方法
    virtual void updateQualityPoints(Crop* crop, bool isWatered, bool isFertilized, float fertility) = 0;
    virtual float getPriceMultiplier() const = 0;
    virtual ~ICropQualityState() = default;
};

#endif // __I_CROP_QUALITY_STATE_H__