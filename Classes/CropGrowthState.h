#pragma once
#ifndef __CROP_GROWTH_STATE_H__
#define __CROP_GROWTH_STATE_H__

class Crop;

class ICropGrowthState {
public:
	virtual ~ICropGrowthState() = default;
	virtual void advance(Crop* crop) = 0;
	virtual GrowthStage getStage() const = 0;
};

class SeedState : public ICropGrowthState {
public:
	void advance(Crop* crop) override;
	GrowthStage getStage() const override { return GrowthStage::SEED; }
};

class SproutState : public ICropGrowthState {
public:
	void advance(Crop* crop) override;
	GrowthStage getStage() const override { return GrowthStage::SPROUT; }
};

class GrowingState : public ICropGrowthState {
public:
	void advance(Crop* crop) override;
	GrowthStage getStage() const override { return GrowthStage::GROWING; }
};

class MatureState : public ICropGrowthState {
public:
	void advance(Crop* crop) override;
	GrowthStage getStage() const override { return GrowthStage::MATURE; }
};
#endif // __CROP_GROWTH_STATE_H__