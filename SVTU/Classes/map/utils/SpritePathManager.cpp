// SpritePathManager.cpp
#include "SpritePathManager.h"

std::string SpritePathManager::getSpritePath(MapType mapType,
                                             const std::string &subType,
                                             const std::string &layerName,
                                             const std::string &season) {
  switch (mapType) {
  case MapType::FARM:
    return getFarmSpritePath(subType, layerName, season);
  case MapType::INDOOR:
    return getIndoorSpritePath(layerName);
  case MapType::COMMUNITY:
    return getCommunitySpritePath(layerName);
  case MapType::MINE:
    return getMineSpritePath(layerName);
  default:
    return "";
  }
}

std::string SpritePathManager::getFarmSpritePath(const std::string &farmType,
                                                 const std::string &layerName,
                                                 const std::string &season) {
  static const std::unordered_map<std::string, std::string> layerToAssetType = {
      {"house", "house/house_1"},           {"myhouse", "myhouse/myhouse_1"},
      {"obj_fb_2", "obj_forbidden/tree_2"}, {"obj_mv_1", "obj_move/bushes_1"},
      {"static_1", "static/size_1"},        {"static_4", "static/size_4"},
      {"static_5", "static/size_5"}};

  auto it = layerToAssetType.find(layerName);
  if (it == layerToAssetType.end()) {
    return "";
  }

  if (isStaticAsset(layerName)) {
    return "maps/farm/" + farmType + "/asset/" + it->second + ".png";
  }

  return "maps/farm/" + farmType + "/asset/" + it->second + "/" + season + "/" +
         layerName + "_1.png";
}

std::string
SpritePathManager::getIndoorSpritePath(const std::string &layerName) {
  return "maps/indoor/asset/" + layerName + ".png";
}

std::string
SpritePathManager::getCommunitySpritePath(const std::string &layerName) {
  return "maps/community/asset/" + layerName + ".png";
}

std::string SpritePathManager::getMineSpritePath(const std::string &layerName) {
  return "maps/mine/asset/" + layerName + ".png";
}