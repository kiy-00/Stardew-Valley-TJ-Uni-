// SpritePathManager.h
#pragma once
#include <string>
#include <unordered_map>

class SpritePathManager {
public:
  // 地图类型枚举
  enum class MapType {
    FARM,      // 农场（包括岛屿和标准农场）
    INDOOR,    // 室内
    COMMUNITY, // 社区
    MINE       // 矿场
  };

  static std::string
  getSpritePath(MapType mapType,            // 地图类型
                const std::string &subType, // 子类型（如"island"或"standard"）
                const std::string &layerName,
                const std::string &season = "spring");

private:
  static std::string getFarmSpritePath(const std::string &farmType,
                                       const std::string &layerName,
                                       const std::string &season);

  static std::string getIndoorSpritePath(const std::string &layerName);
  static std::string getCommunitySpritePath(const std::string &layerName);
  static std::string getMineSpritePath(const std::string &layerName);

  static bool isStaticAsset(const std::string &layerName) {
    return layerName.find("static_") == 0;
  }
};