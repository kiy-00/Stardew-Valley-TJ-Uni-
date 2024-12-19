#include "ILandState.h"
#include "LandStates.h"
#include "Farmland.h"

void ILandState::updateWaterTimer(Farmland* land, float dt) {
	land->setWaterTimer(land->getWaterTimer() + dt);
}
void ILandState::updateFertilizeTimer(Farmland* land, float dt) {
	land->setFertilizeTimer(land->getFertilizeTimer() + dt);
}

void ILandState::transitionToTilledState(Farmland* land) {
	land->setWaterTimer(0.0f);
	land->setFertilizeTimer(0.0f);
	land->setState(new TilledState());
	CCLOG("Both water and fertilizer depleted");
}
void ILandState::transitionToWateredState(Farmland* land) {
	land->setFertilizeTimer(0.0f);
	land->setState(new WateredState());
	CCLOG("Fertilizer depleted from watered land");
}
void ILandState::transitionToFertilizedState(Farmland* land) {
	land->setWaterTimer(0.0f);
	land->setState(new FertilizedState());
	CCLOG("Water evaporated from fertilized land");
}