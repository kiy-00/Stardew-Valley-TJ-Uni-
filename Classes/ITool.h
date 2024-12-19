#pragma once
#ifndef __I_TOOL_H__
#define __I_TOOL_H__

#include "cocos2d.h"
#include "Farmland.h"

class ITool {
public:
	virtual ~ITool() = default;
	virtual bool useTool(Farmland* land) = 0;
	virtual ToolType getToolName() const = 0;
};

#endif // __I_TOOL_H__