#pragma once
#ifndef __I_SEASON_OBSERVER__
#define __I_SEASON_OBSERVER__

class SeasonSystem;
class ISeasonObserver {
public:
    virtual ~ISeasonObserver() = default;
    virtual void onSeasonChanged(SeasonSystem* seasonSystem) = 0;
};

#endif // !_I_WEATHER_OBSERVER_