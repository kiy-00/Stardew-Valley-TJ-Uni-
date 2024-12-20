// Hoe.h
#pragma once
#ifndef __HOE_H__
#define __HOE_H__

#include "ITool.h"


class Hoe : public ITool {
public:
	bool useTool(Farmland* land) override {
		if (land->isTilled()) {
			CCLOG("Land is already tilled!");
			return false;
		}
		land->till();
		return true;
	}

	ToolType getToolName() const override {
		return ToolType::HOE;
	}
};

#endif // __HOE_H__