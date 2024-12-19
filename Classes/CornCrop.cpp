#include "CornCrop.h"

USING_NS_CC;


bool CornCrop::init() {

	initializeGrowthStages();

	return true;
}

void CornCrop::initializeGrowthStages() {
	m_stageTextures = {
		{GrowthStage::SEED, "crop/corn/corn_seed.png"},
		{GrowthStage::SPROUT, "crop/corn/corn_sprout.png"},
		{GrowthStage::GROWING, "crop/corn/corn_growing.png"},
		{GrowthStage::MATURE, "crop/corn/corn_mature.png"},
		{GrowthStage::DEAD,"crop/corn/corn_dead.png"}
	};

	// …Ë÷√≥ı ºÃ˘Õº
	setTexture(m_stageTextures[GrowthStage::SEED]);
}