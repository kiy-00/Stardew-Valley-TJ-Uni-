﻿// filepath: map/utils/LayerRendererFactory.cpp

#include "LayerRendererFactory.h"



std::string LayerRendererFactory::getSpritePath(const std::string &layerName) {

  static std::unordered_map<std::string, std::string> layerToSpritePath = {

      {"house", "maps/farm/island/asset/house/house_1/spring/house_1_1.png"},

      {"myhouse",

       "maps/farm/island/asset/myhouse/myhouse_1/spring/myhouse_1_1.png"},

      {"obj_fb_2",

       "maps/farm/island/asset/obj_forbidden/tree_2/spring/tree_2_1.png"},

      {"obj_mv_1",

       "maps/farm/island/asset/obj_move/bushes_1/spring/bushes_1_1.png"},

      {"static_1", "maps/farm/island/asset/static/size_1/boat.png"},

      {"static_4", "maps/farm/island/asset/static/size_4/floor.png"},

      {"static_5", "maps/farm/island/asset/static/size_5/bushes.png"}};



  auto it = layerToSpritePath.find(layerName);

  if (it != layerToSpritePath.end()) {

    return it->second;

  }

  return "";

}