#ifndef __CORN_CROP_H__
#define __CORN_CROP_H__

#include "Crop.h"
#include <vector>

class CornCrop : public Crop {
public:
	CornCrop() {init(); };
	CREATE_FUNC(CornCrop);
	virtual bool init() override;

	// 作物特性
	virtual int getBaseValue() const override { return 150; }
	virtual int getGrowthDays() const override { return 14; }
	virtual int getHarvestValue() const override { return 0; }
	virtual CropType getCropType() const override { return CropType::CORN; }  // 修改返回类型
	virtual std::vector<SeasonType> getSupportSeason() const override { return {SeasonType::SUMMER}; }

protected:
	virtual void initializeGrowthStages() override;
};

#endif // __CORN_CROP_H__