// filepath: map/utils/LayerRendererFactory.cpp
#include "LayerRendererFactory.h"

std::string LayerRendererFactory::getSpritePath(const std::string &layerName) {
  static std::unordered_map<std::string, std::string> layerToSpritePath = {
      {"house", "maps/farm/island/house_1_spring.png"},
      {"myhouse", "maps/farm/island/myhouse_1_spring.png"},
      {"obj_fb_2", "maps/farm/island/tree_2_spring.png"},
      {"obj_mv_1", "maps/farm/island/bushes_1_spring.png"},
      {"static_1", "maps/farm/island/st_size1.png"},
      {"static_4", "maps/farm/island/s4_floor.png"},
      {"static_5", "maps/farm/island/s5_bushes.png"}};

  auto it = layerToSpritePath.find(layerName);
  if (it != layerToSpritePath.end()) {
    return it->second;
  } else {
    return "";
  }
}