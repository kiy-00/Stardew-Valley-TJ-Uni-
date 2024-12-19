// ConcreteToolFactories.h
#pragma once
#ifndef __CONCRETE_TOOL_FACTORIES_H__
#define __CONCRETE_TOOL_FACTORIES_H__

#include "ToolFactory.h"
#include "Hoe.h"
#include "WateringCan.h"
#include "Fertilizer.h"
#include "Pesticide.h"
#include "Sickle.h"
class HoeFactory : public ToolFactory {
public:
	ITool* createTool() override {
		return new Hoe();
	}
};

class WateringCanFactory : public ToolFactory {
public:
	ITool* createTool() override {
		return new WateringCan();
	}
};

class FertilizerFactory : public ToolFactory {
public:
	ITool* createTool() override {
		return new Fertilizer();
	}
};

class PesticideFactory : public ToolFactory {
public:
	ITool* createTool() override {
		return new Pesticide();
	}
};

class SickleFactory : public ToolFactory {
public:
	ITool* createTool() override {
		return new Sickle();
	}
};

#endif // __CONCRETE_TOOL_FACTORIES_H__#pragma once
