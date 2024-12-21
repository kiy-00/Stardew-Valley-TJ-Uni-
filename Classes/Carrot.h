#pragma once
#include "Crop.h"

class Carrot : public Crop {
public:
    static Carrot* create();
    virtual bool init() override;

protected:
    virtual void updateGrowth(float dt) override;
    virtual bool checkGrowthConditions() override;
};