// Tile.cpp
#include "Tile.h"

namespace MapSystem {

Tile::Tile()
    : m_gid(0), m_flags(0), m_state(MapConstants::TileState::NORMAL),
      m_currentSeason(MapConstants::Season::SPRING), m_stateTimer(0.0f) {}

void Tile::onSeasonChanged(MapConstants::Season newSeason) {
  if (m_currentSeason != newSeason) {
    m_currentSeason = newSeason;

    // 根据季节变化更新瓦片状态
    switch (m_state) {
    case MapConstants::TileState::GROWING:
    case MapConstants::TileState::MATURE:
      // 可能需要根据季节调整生长速度或状态
      updateState();
      break;
    default:
      break;
    }
  }
}

void Tile::till() {
  if (isTillable() && m_state == MapConstants::TileState::NORMAL) {
    m_state = MapConstants::TileState::TILLED;
    updateState();
  }
}

void Tile::water() {
  if (isWaterable() && m_state == MapConstants::TileState::TILLED) {
    m_state = MapConstants::TileState::WATERED;
    updateState();
  }
}

void Tile::plant() {
  if (isPlantable() && (m_state == MapConstants::TileState::TILLED ||
                        m_state == MapConstants::TileState::WATERED)) {
    m_state = MapConstants::TileState::PLANTED;
    m_stateTimer = 0.0f;
    updateState();
  }
}

void Tile::harvest() {
  if (m_state == MapConstants::TileState::MATURE) {
    m_state = MapConstants::TileState::TILLED;
    updateState();
  }
}

void Tile::update(float dt) {
  switch (m_state) {
  case MapConstants::TileState::PLANTED:
  case MapConstants::TileState::GROWING:
    m_stateTimer += dt;
    updateState();
    break;
  default:
    break;
  }
}

void Tile::updateState() {
  // 验证当前状态的合法性
  validateState();

  // 处理状态转换
  switch (m_state) {
  case MapConstants::TileState::PLANTED:
    // 检查是否应该开始生长
    if (m_stateTimer >= 1.0f) { // 假设1秒后开始生长
      m_state = MapConstants::TileState::GROWING;
      m_stateTimer = 0.0f;
    }
    break;

  case MapConstants::TileState::GROWING:
    // 检查是否应该成熟
    if (m_stateTimer >= 10.0f) { // 假设10秒后成熟
      m_state = MapConstants::TileState::MATURE;
      m_stateTimer = 0.0f;
    }
    break;

  default:
    break;
  }
}

void Tile::validateState() {
  // 确保状态和标志位的一致性
  if (m_state != MapConstants::TileState::NORMAL) {
    if (!isTillable()) {
      m_state = MapConstants::TileState::NORMAL;
      m_stateTimer = 0.0f;
    }
  }

  if (m_state == MapConstants::TileState::PLANTED ||
      m_state == MapConstants::TileState::GROWING ||
      m_state == MapConstants::TileState::MATURE) {
    if (!isPlantable()) {
      m_state = MapConstants::TileState::NORMAL;
      m_stateTimer = 0.0f;
    }
  }
}

} // namespace MapSystem
