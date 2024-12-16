// Tile.h
#ifndef __TILE_H__
#define __TILE_H__

#include "MapConstants.h"
#include "cocos2d.h"
#include <bitset>

USING_NS_CC;
namespace MapSystem {
class Tile {
public:
  Tile();
  ~Tile() = default;

  // 基础属性设置
  void setPosition(const Vec2 &pos) { m_position = pos; }
  const Vec2 &getPosition() const { return m_position; }

  void setGID(int gid) { m_gid = gid; }
  int getGID() const { return m_gid; }

  // 状态管理
  void setState(MapConstants::TileState state) { m_state = state; }
  MapConstants::TileState getState() const { return m_state; }

  // 标志位操作
  void setFlag(uint32_t flag) { m_flags |= flag; }
  void clearFlag(uint32_t flag) { m_flags &= ~flag; }
  bool checkFlag(uint32_t flag) const { return (m_flags & flag) == flag; }

  // 季节变化响应
  void onSeasonChanged(MapConstants::Season newSeason);

  // 属性检查快捷方法
  bool isWalkable() const {
    return checkFlag(MapConstants::TileFlags::WALKABLE);
  }
  bool isTillable() const {
    return checkFlag(MapConstants::TileFlags::TILLABLE);
  }
  bool isWaterable() const {
    return checkFlag(MapConstants::TileFlags::WATERABLE);
  }
  bool isPlantable() const {
    return checkFlag(MapConstants::TileFlags::PLANTABLE);
  }

  // 状态变化
  void till();    // 耕地
  void water();   // 浇水
  void plant();   // 种植
  void harvest(); // 收获

  // 更新
  void update(float dt);

private:
  Vec2 m_position;                      // 瓦片在地图中的位置
  int m_gid;                            // 瓦片的图块ID
  uint32_t m_flags;                     // 瓦片标志位
  MapConstants::TileState m_state;      // 瓦片当前状态
  MapConstants::Season m_currentSeason; // 当前季节

  float m_stateTimer; // 状态计时器（用于生长等状态变化）

  // 内部工具方法
  void updateState();
  void validateState();
};
} // namespace MapSystem

#endif // __TILE_H__