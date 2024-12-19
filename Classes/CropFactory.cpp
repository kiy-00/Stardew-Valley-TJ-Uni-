#include "CropFactory.h"
#include "CornCrop.h"
#include "PotatoCrop.h"
#include "WheatCrop.h"
#include "TomatoCrop.h"
#include "CarrotCrop.h"

CropFactory* CropFactory::s_instance = nullptr;
const std::unordered_map<CropType, std::function<Crop* ()>> CropFactory::cropCreators = {
{CropType::CORN, []() { return CornCrop::create(); }},
{CropType::WHEAT, []() { return WheatCrop::create(); }},
{CropType::POTATO, []() { return PotatoCrop::create(); }},
{CropType::TOMATO, []() { return TomatoCrop::create(); } },
{CropType::CARROT, []() { return CarrotCrop::create(); } }
};

CropFactory* CropFactory::getInstance() {
	if (!s_instance) {
		s_instance = new CropFactory();
	}
	return s_instance;
}

Crop* CropFactory::createCrop(CropType type) {
    auto it = cropCreators.find(type);
    if (it != cropCreators.end()) {
        return it->second();
    }
    CCLOG("Unknown crop type!");
    return nullptr;
}