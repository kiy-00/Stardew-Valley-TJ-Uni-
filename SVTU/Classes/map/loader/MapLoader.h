#include "MapConstants.h"

#include "cocos2d.h"

#include <set>



class MapLoader {

public:

  // 加载TMX地图

  cocos2d::TMXTiledMap *loadTMXMap(const std::string &tmxPath);



  // 预加载贴图资源

  bool preloadMapResources(const std::string &tmxPath);



  // 释放资源

  void unloadMapResources(const std::string &tmxPath);



private:

  // 检查地图文件有效性

  bool validateTMXFile(const std::string &tmxPath);



  // 加载地图所需的贴图集

  bool loadTilesets(cocos2d::TMXTiledMap *map);



  // 收集纹理路径

  std::set<std::string> collectTexturePaths(cocos2d::TMXTiledMap *map);

};