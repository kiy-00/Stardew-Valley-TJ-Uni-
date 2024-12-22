// SpritePathManager.cpp

#include "SpritePathManager.h"

std::string SpritePathManager::getFarmSpritePath(const std::string &layerName,

                                                 const std::string &farmType,

                                                 const std::string &season) {

  // 处理静态资源

  if (isStaticLayer(layerName)) {

    if (layerName == "static_1") {

      return "maps/farm/" + farmType + "/asset/static/size_1/boat.png";
    }

    if (layerName == "static_4") {

      return "maps/farm/" + farmType + "/asset/static/size_4/floor.png";
    }

    if (layerName == "static_5") {

      return "maps/farm/" + farmType + "/asset/static/size_5/bushes.png";
    }

    if (layerName == "static_2") {

      return "maps/farm/" + farmType + "/asset/static/size_2/stone.png";
    }

    return "";
  }

  // 处理季节性资源

  if (layerName == "house") {

    return "maps/farm/" + farmType + "/asset/house/house_1/" + season +

           "/house_1_1.png";
  }

  if (layerName == "myhouse") {

    return "maps/farm/" + farmType + "/asset/myhouse/myhouse_1/" + season +

           "/myhouse_1_1.png";
  }

  if (layerName == "obj_fb_2") {

    return "maps/farm/" + farmType + "/asset/obj_forbidden/tree_2/" + season +

           "/tree_2_1.png";
  }

  if (layerName == "obj_mv_1") {

    return "maps/farm/" + farmType + "/asset/obj_move/bushes_1/" + season +

           "/bushes_1_1.png";
  }

  return "";
}

bool SpritePathManager::isStaticLayer(const std::string &layerName) {

  return layerName.find("static_") == 0;
}

std::string SpritePathManager::getCaveSpritePath(const std::string &layerName) {

  if (layerName == "red_diamond_ore") {

    return "maps/cave/asset/prize/red_diamond_ore.png";
  }

  if (layerName == "green_diamond_ore") {

    return "maps/cave/asset/prize/green_diamond_ore.png";
  }

  if (layerName == "lapiz_ore") {

    return "maps/cave/asset/prize/lapiz_ore.png";
  }

  if (layerName == "gold_ore") {

    return "maps/cave/asset/prize/gold_ore.png";
  }

  if (layerName == "iron_ore") {

    return "maps/cave/asset/prize/iron_ore.png";
  }

  if (layerName == "magafruit") {

    return "maps/cave/asset/prize/magafruit.png";
  }

  if (layerName == "magaheart") {

    return "maps/cave/asset/prize/magaheart.png";
  }

  if (layerName == "monster_sleep") {

    return "maps/cave/asset/monster/monster_sleep.png";
  }

  if (layerName == "monster_awake") {

    return "maps/cave/asset/monster/monster_awake.png";
  }

  return "";
}

std::string
SpritePathManager::getInsideSpritePath(const std::string &layerName) {

  if (layerName == "bed") {

    return "maps/inside/asset/useful_items/bed_2.png";
  }

  if (layerName == "red_sofa_left") {

    return "maps/inside/asset/useful_items/red_sofa_left.png";
  }

  if (layerName == "black_sofa") {

    return "maps/inside/asset/useful_items/black_sofa.png";
  }

  if (layerName == "blue_sofa") {

    return "maps/inside/asset/useful_items/blue_sofa.png";
  }

  if (layerName == "dark_fireplace") {

    return "maps/inside/asset/decorates/dark_fireplace.png";
  }

  if (layerName == "tv_back") {

    return "maps/inside/asset/decorates/tv_back.png";
  }

  if (layerName == "dark_bookshelf") {

    return "maps/inside/asset/decorates/dark_bookshelf.png";
  }

  if (layerName == "shelf_1") {

    return "maps/inside/asset/decorates/shelf_1.png";
  }

  if (layerName == "shelf_2") {

    return "maps/inside/asset/decorates/shelf_2.png";
  }

  if (layerName == "shelf_3") {

    return "maps/inside/asset/decorates/shelf_3.png";
  }

  if (layerName == "shelf_4") {

    return "maps/inside/asset/decorates/shelf_4.png";
  }

  if (layerName == "round_table") {

    return "maps/inside/asset/decorates/round_table.png";
  }

  if (layerName == "chair") {

    return "maps/inside/asset/decorates/chair.png";
  }

  if (layerName == "chair_2") {

    return "maps/inside/asset/decorates/chair_2.png";
  }

  return "";
}