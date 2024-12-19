// ToolFactory.h
#pragma once
#ifndef __TOOL_FACTORY_H__
#define __TOOL_FACTORY_H__

#include "ITool.h"

class ToolFactory {
public:
	virtual ~ToolFactory() = default;
	virtual ITool* createTool() = 0;  // 工厂方法
};

#endif // __TOOL_FACTORY_H__