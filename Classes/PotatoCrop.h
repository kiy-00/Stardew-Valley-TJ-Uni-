// PotatoCrop.h
#ifndef __POTATO_CROP_H__
#define __POTATO_CROP_H__
#include "Crop.h"

class PotatoCrop : public Crop {
public:
    PotatoCrop() { init(); }
    CREATE_FUNC(PotatoCrop);
    virtual bool init() override;

    virtual int getBaseValue() const override { return 120; }
    virtual int getGrowthDays() const override { return 12; }
    virtual int getHarvestValue() const override { return 0; }
    virtual CropType getCropType() const override { return CropType::POTATO; }
    virtual std::vector<SeasonType> getSupportSeason() const override {
        return { SeasonType::SPRING, SeasonType::SUMMER };
    }
protected:
    virtual void initializeGrowthStages() override;
};
#endif
