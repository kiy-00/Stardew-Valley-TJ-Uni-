// SpritePathManager.cpp
#include "SpritePathManager.h"

std::string SpritePathManager::getFarmSpritePath(const std::string &layerName,
                                                 const std::string &farmType,
                                                 const std::string &season) {

    if (farmType == "island") {
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
            return "maps/farm/" + farmType + "/asset/house/house_1/" + season + "/house_1_1.png";
        }
        if (layerName == "myhouse") {
            return "maps/farm/" + farmType + "/asset/myhouse/myhouse_1/" + season + "/myhouse_1_1.png";
        }
        if (layerName == "obj_fb_2") {
            return "maps/farm/" + farmType + "/asset/obj_forbidden/tree_2/" + season + "/tree_2_1.png";
        }
        if (layerName == "obj_mv_1") {
            return "maps/farm/" + farmType + "/asset/obj_move/bushes_1/" + season + "/bushes_1_1.png";
        }

    }

    if (farmType == "standard") {
        // 处理静态资源
        if (isStaticLayer(layerName)) {
            if (layerName == "static_2") {
                return "maps/farm/" + farmType + "/asset/static/size_2/stone.png";
            }
            if (layerName == "static_3") {
                return "maps/farm/" + farmType + "/asset/static/size_3/stone.png";
            }
            if (layerName == "static_4") {
                return "maps/farm/" + farmType + "/asset/static/size_4/floor.png";
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
            return "maps/farm/" + farmType + "/asset/obj_forbidden/tree_1/" + season +
                "/tree_1_1.png";
        }
        if (layerName == "obj_mv_1") {
            return "maps/farm/" + farmType + "/asset/obj_move/bushes_1/" + season +
                "/bushes_1_1.png";
        }


    }

  


  return "";
}

bool SpritePathManager::isStaticLayer(const std::string &layerName) {
  return layerName.find("static_") == 0;
}