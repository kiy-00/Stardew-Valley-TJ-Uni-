#pragma once
#ifndef __THREAT_STATES_H__
#define __THREAT_STATES_H__
#include "IThreatState.h"

class DroughtThreatState : public IThreatState {
private:
    float m_timer = 0;
    const float DAMAGE_INTERVAL = 1.0f;  // 每秒伤害一次
    const float DAMAGE_AMOUNT = 5.0f;    // 每次伤害量
public:
    void update(Crop* crop, float dt) override;
    ThreatType getType() const override { return ThreatType::DROUGHT; }
    void cure() override { m_timer = 0; }
};

class DiseaseThreatState : public IThreatState {
private:
    float m_timer = 0;
    const float DAMAGE_INTERVAL = 2.0f;
    const float DAMAGE_AMOUNT = 10.0f;

public:
    void update(Crop* crop, float dt) override;
    ThreatType getType() const override { return ThreatType::DISEASE; }
    void cure() override { m_timer = 0; }
};

#endif