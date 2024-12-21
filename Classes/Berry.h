#pragma 
#include "Crop.h"

class Berry : public Crop {
public:
    static Berry* create();
    virtual bool init() override;

protected:
    virtual void updateGrowth(float dt) override;
    virtual bool checkGrowthConditions() override;
};
