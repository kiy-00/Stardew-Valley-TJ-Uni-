// CarrotCrop.h
#ifndef __CARROT_CROP_H__
#define __CARROT_CROP_H__
#include "Crop.h"

class CarrotCrop : public Crop {
public:
    CarrotCrop() { init(); }
    CREATE_FUNC(CarrotCrop);
    virtual bool init() override;

    virtual int getBaseValue() const override { return 90; }
    virtual int getGrowthDays() const override { return 8; }
    virtual int getHarvestValue() const override { return 0; }
    virtual CropType getCropType() const override { return CropType::CARROT; }
    virtual std::vector<SeasonType> getSupportSeason() const override {
        return { SeasonType::SPRING, SeasonType::AUTUMN };
    }
protected:
    virtual void initializeGrowthStages() override;
};
#endif