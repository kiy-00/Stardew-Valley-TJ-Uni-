// ToolManager.h
#pragma once
#ifndef __TOOL_MANAGER_H__
#define __TOOL_MANAGER_H__

#include "cocos2d.h"
#include "ITool.h"
#include "ConcreteToolFactories.h"
#include <map>

class ToolManager {
public:
	static ToolManager* getInstance() {
		static ToolManager instance;
		return &instance;
	}

	ITool* createTool(const std::string& toolType) {
		auto it = m_factories.find(toolType);
		if (it != m_factories.end()) {
			return it->second->createTool();
		}
		CCLOG("Unknown tool type: %s", toolType.c_str());
		return nullptr;
	}

private:
	std::map<std::string, ToolFactory*> m_factories;

	ToolManager() {
		// 注册工厂
		m_factories["hoe"] = new HoeFactory();
		m_factories["wateringcan"] = new WateringCanFactory();
		m_factories["fertilizer"] = new FertilizerFactory();
		m_factories["pesticide"] = new PesticideFactory();
		m_factories["sickle"] = new SickleFactory();
	}

	~ToolManager() {
		// 清理工厂
		for (auto& pair : m_factories) {
			delete pair.second;
		}
	}
};

#endif // __TOOL_MANAGER_H__