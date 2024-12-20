#pragma once

#ifndef __PESTICIDE_H__
#define __PESTICIDE_H__

#include "ITool.h"
#include "Crop.h"
class Pesticide : public ITool {
public:
    bool useTool(Farmland* land) override {
        Crop* crop = land->getCrop();
        if (crop&&!crop->isDead()&&crop->getThreatType() == ThreatType::DISEASE) {
            crop->setThreatState(nullptr);
            return true;
        }
        return false;
    }
    ToolType getToolName() const override {
        return ToolType::PESTICIDE;
    }

};

#endif