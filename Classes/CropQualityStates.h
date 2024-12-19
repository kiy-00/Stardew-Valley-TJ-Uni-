#pragma once
#ifndef __CROP_QUALITY_STATES_H__
#define __CROP_QUALITY_STATES_H__
#include "ICropQualityState.h"
class NormalQualityState : public ICropQualityState {
public:
	CropQuality getQuality() const override { return CropQuality::NORMAL; }
	void updateQualityPoints(Crop* crop, bool isWatered, bool isFertilized, float fertility) override;
	float getPriceMultiplier() const override {
		return 1.0f;
	}
};

class SilverQualityState : public ICropQualityState {
public:
	CropQuality getQuality() const override { return CropQuality::SILVER; }
	void updateQualityPoints(Crop* crop, bool isWatered, bool isFertilized, float fertility) override;
	float getPriceMultiplier() const override {
		return 1.25f;  // 银品质加价25%
	}
};

class GoldQualityState : public ICropQualityState {
public:
	CropQuality getQuality() const override { return CropQuality::GOLD; }
	void updateQualityPoints(Crop* crop, bool isWatered, bool isFertilized, float fertility) override;
	float getPriceMultiplier() const override {
		return 1.5f;
	}
};
#endif