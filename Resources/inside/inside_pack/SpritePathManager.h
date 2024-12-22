// SpritePathManager.h
#pragma once
#include <string>

class SpritePathManager {
public:
  // 只保留农场相关的接口
  static std::string getFarmSpritePath(
      const std::string &layerName, // 层名称，如 "house", "obj_fb_2"
      const std::string &farmType,  // 农场类型，如 "island"
      const std::string &season     // 季节，如 "spring"
  );
  static std::string getCaveSpritePath(

      const std::string &layerName // 层名称，如 "house", "obj_fb_2"

  );

  static std::string getInsideSpritePath(

      const std::string &layerName // 层名称，如 "house", "obj_fb_2"

  );

private:
  static bool isStaticLayer(const std::string &layerName);
};
