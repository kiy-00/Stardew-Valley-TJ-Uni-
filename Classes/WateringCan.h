#pragma once
#ifndef __WATERING_CAN_H__
#define __WATERING_CAN_H__

#include "ITool.h"

class WateringCan : public ITool {
public:
	bool useTool(Farmland* land) override {
		if (!land->isTilled()) {
			CCLOG("Cannot water untilled land!");
			return false;
		}
		if (land->isWatered()) {
			CCLOG("Land is already watered!");
			return false;
		}
		land->water();
		return true;
	}

	ToolType getToolName() const override {
		return ToolType::WATERING_CAN;
	}
};

#endif // __WATERING_CAN_H__