#include "MapLoader.h"
#include <set>

// Helper function to collect texture paths from layers and object groups
std::set<std::string>
MapLoader::collectTexturePaths(cocos2d::TMXTiledMap *map) {
  std::set<std::string> texturePaths;

  // 遍历所有子节点，查找图层和对象组
  auto children = map->getChildren();
  for (auto &child : children) {
    if (auto layer = dynamic_cast<cocos2d::TMXLayer *>(child)) {
      auto tilesetInfo = layer->getTileSet();
      if (tilesetInfo) {
        // texturePaths.insert(tilesetInfo->sourceImage);
      }
    } else if (auto objectGroup =
                   dynamic_cast<cocos2d::TMXObjectGroup *>(child)) {
      // 如果对象组使用 Tileset，可以在此处添加处理逻辑
      // 示例：
      // auto tilesetInfo = objectGroup->getTileSet();
      // if (tilesetInfo) {
      //     texturePaths.insert(tilesetInfo->sourceImage);
      // }
    }
  }

  return texturePaths;
}

cocos2d::TMXTiledMap *MapLoader::loadTMXMap(const std::string &tmxPath) {
  // 验证地图文件
  if (!validateTMXFile(tmxPath)) {
    cocos2d::log("Invalid TMX file: %s", tmxPath.c_str());
    return nullptr;
  }

  // 加载TMX地图
  auto map = cocos2d::TMXTiledMap::create(tmxPath);
  if (!map) {
    cocos2d::log("Failed to create TMX map: %s", tmxPath.c_str());
    return nullptr;
  }

  // 加载贴图集
  // if (!loadTilesets(map)) {
  //   cocos2d::log("Failed to load tilesets for map: %s", tmxPath.c_str());
  //   return nullptr;
  // }

  return map;
}

bool MapLoader::preloadMapResources(const std::string &tmxPath) {
  // 创建一个临时的TMX地图对象来获取资源信息
  auto map = cocos2d::TMXTiledMap::create(tmxPath);
  if (!map)
    return false;

  // 预加载所有贴图集
  return loadTilesets(map);
}

void MapLoader::unloadMapResources(const std::string &tmxPath) {
  // 创建临时TMX地图对象
  auto map = cocos2d::TMXTiledMap::create(tmxPath);
  if (!map)
    return;

  // 从纹理缓存中移除贴图
  auto texturePaths = collectTexturePaths(map);

  for (const auto &texturePath : texturePaths) {
    cocos2d::Director::getInstance()->getTextureCache()->removeTextureForKey(
        texturePath);
  }
}

bool MapLoader::validateTMXFile(const std::string &tmxPath) {
  // 检查文件是否存在
  if (!cocos2d::FileUtils::getInstance()->isFileExist(tmxPath)) {
    return false;
  }

  // 验证文件格式（可以添加更多验证）
  std::string extension =
      cocos2d::FileUtils::getInstance()->getFileExtension(tmxPath);
  return extension == ".tmx";
}

bool MapLoader::loadTilesets(cocos2d::TMXTiledMap *map) {
  if (!map)
    return false;

  auto texturePaths = collectTexturePaths(map);

  for (const auto &texturePath : texturePaths) {
    if (!cocos2d::Director::getInstance()->getTextureCache()->addImage(
            texturePath)) {
      return false;
    }
  }

  return true;
}