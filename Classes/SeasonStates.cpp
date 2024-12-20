#include "SeasonStates.h"

void SpringState::changeToNextSeason(SeasonSystem* seasonSystem)const {
	seasonSystem->setCurrentSeason(new SummerState());
}

void SummerState::changeToNextSeason(SeasonSystem* seasonSystem)const {
	seasonSystem->setCurrentSeason(new AutumnState());
}

void AutumnState::changeToNextSeason(SeasonSystem* seasonSystem)const {
	seasonSystem->setCurrentSeason(new WinterState());
}

void WinterState::changeToNextSeason(SeasonSystem* seasonSystem)const {
	seasonSystem->setCurrentSeason(new SpringState());
}