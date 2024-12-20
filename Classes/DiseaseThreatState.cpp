#include "ThreatStates.h"
#include "Crop.h"

void DiseaseThreatState::update(Crop* crop, float dt) {
    m_timer += dt;
    if (m_timer >= DAMAGE_INTERVAL) {
        crop->onDamage(DAMAGE_AMOUNT);
        m_timer = 0;
    }
}