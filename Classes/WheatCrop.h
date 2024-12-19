// WheatCrop.h
#ifndef __WHEAT_CROP_H__
#define __WHEAT_CROP_H__
#include "Crop.h"

class WheatCrop : public Crop {
public:
    WheatCrop() { init(); }
    CREATE_FUNC(WheatCrop);
    virtual bool init() override;

    virtual int getBaseValue() const override { return 100; }
    virtual int getGrowthDays() const override { return 10; }
    virtual int getHarvestValue() const override { return 0; }
    virtual CropType getCropType() const override { return CropType::WHEAT; }
    virtual std::vector<SeasonType> getSupportSeason() const override {
        return { SeasonType::SPRING, SeasonType::AUTUMN };
    }
protected:
    virtual void initializeGrowthStages() override;
};
#endif