#include "CarrotCrop.h"

USING_NS_CC;


bool CarrotCrop::init() {

	initializeGrowthStages();

	return true;
}

void CarrotCrop::initializeGrowthStages() {
	m_stageTextures = {
		{GrowthStage::SEED, "crop/carrot/carrot_seed.png"},
		{GrowthStage::SPROUT, "crop/carrot/carrot_sprout.png"},
		{GrowthStage::GROWING, "crop/carrot/carrot_growing.png"},
		{GrowthStage::MATURE, "crop/carrot/carrot_mature.png"},
		{GrowthStage::DEAD,"crop/carrot/carrot_dead.png"}
	};

	// …Ë÷√≥ı ºÃ˘Õº
	setTexture(m_stageTextures[GrowthStage::SEED]);
}