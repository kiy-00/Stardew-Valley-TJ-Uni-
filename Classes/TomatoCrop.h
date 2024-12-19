// TomatoCrop.h
#ifndef __TOMATO_CROP_H__
#define __TOMATO_CROP_H__
#include "Crop.h"

class TomatoCrop : public Crop {
public:
    TomatoCrop() { init(); }
    CREATE_FUNC(TomatoCrop);
    virtual bool init() override;

    virtual int getBaseValue() const override { return 180; }
    virtual int getGrowthDays() const override { return 16; }
    virtual int getHarvestValue() const override { return 0; }
    virtual CropType getCropType() const override { return CropType::TOMATO; }
    virtual std::vector<SeasonType> getSupportSeason() const override {
        return { SeasonType::SUMMER };
    }
protected:
    virtual void initializeGrowthStages() override;
};
#endif