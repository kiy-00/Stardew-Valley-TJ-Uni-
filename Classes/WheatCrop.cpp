#include "WheatCrop.h"

USING_NS_CC;


bool WheatCrop::init() {

	initializeGrowthStages();

	return true;
}

void WheatCrop::initializeGrowthStages() {
	m_stageTextures = {
		{GrowthStage::SEED, "crop/wheat/wheat_seed.png"},
		{GrowthStage::SPROUT, "crop/wheat/wheat_sprout.png"},
		{GrowthStage::GROWING, "crop/wheat/wheat_growing.png"},
		{GrowthStage::MATURE, "crop/wheat/wheat_mature.png"},
		{GrowthStage::DEAD,"crop/wheat/wheat_dead.png"}
	};

	// …Ë÷√≥ı ºÃ˘Õº
	setTexture(m_stageTextures[GrowthStage::SEED]);
}