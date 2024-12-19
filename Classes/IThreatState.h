#pragma once
#ifndef __I_THREAT_STATE_H__
#define __I_THREAT_STATE_H__
#include "Enums.h"

class Crop;
class IThreatState {
public:
    virtual void update(Crop* crop, float dt) = 0;
    virtual ThreatType getType() const = 0;
    virtual void cure() = 0;
    virtual ~IThreatState() = default;
};
#endif // !1

