#include "TomatoCrop.h"

USING_NS_CC;


bool TomatoCrop::init() {

	initializeGrowthStages();

	return true;
}

void TomatoCrop::initializeGrowthStages() {
	m_stageTextures = {
		{GrowthStage::SEED, "crop/tomato/tomato_seed.png"},
		{GrowthStage::SPROUT, "crop/tomato/tomato_sprout.png"},
		{GrowthStage::GROWING, "crop/tomato/tomato_growing.png"},
		{GrowthStage::MATURE, "crop/tomato/tomato_mature.png"},
		{GrowthStage::DEAD,"crop/tomato/tomato_dead.png"}
	};

	// …Ë÷√≥ı ºÃ˘Õº
	setTexture(m_stageTextures[GrowthStage::SEED]);
}