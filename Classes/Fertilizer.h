#pragma once
#ifndef __FERTILIZER_H__
#define __FERTILIZER_H__

#include "ITool.h"

class Fertilizer : public ITool {
public:
	bool useTool(Farmland* land) override {
		if (!land->isTilled()) {
			CCLOG("Cannot fertilize untilled land!");
			return false;
		}
		if (land->isFertilized()) {
			CCLOG("Land is already fertilized!");
			return false;
		}
		land->fertilize();
		return true;
	}

	ToolType getToolName() const override {
		return ToolType::FERTILIZER;
	}
};

#endif // __FERTILIZER_H__