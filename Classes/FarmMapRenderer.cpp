// FarmMapRenderer.cpp
#include "FarmMapRenderer.h"
#include "FarmConfigManager.h"
#include "LayerRenderer.h"
#include "SpritePathManager.h"

static FarmMapRenderer *instance = nullptr;

FarmMapRenderer *FarmMapRenderer::getInstance() {
  if (!instance) {
    instance = new FarmMapRenderer();
  }
  return instance;
}

void FarmMapRenderer::renderMap(TMXTiledMap *map, const std::string &season) {
  if (!map) {
    CCLOG("地图未正确加载。");
    return;
  }

  renderTileLayers(map, season);
  renderObjectLayers(map, season);
  renderStaticObjectLayers(map, season);
}

void FarmMapRenderer::renderTileLayers(TMXTiledMap *map, const std::string &season) {
  // 处理基础图层的渲染
  auto configManager = FarmConfigManager::getInstance();
  if (!configManager)
    return;

  try {
    auto config = configManager->getFarmConfig("island");
  } catch (const std::exception &e) {
    CCLOG("渲染图块层时发生错误: %s", e.what());
  }
}

void FarmMapRenderer::renderObjectLayers(TMXTiledMap *map, const std::string &season) {
  // 获取农场配置
  auto configManager = FarmConfigManager::getInstance();
  if (!configManager) {
    CCLOG("FarmConfigManager实例获取失败");
    return;
  }

  FarmTypeConfig config;
  try {
    config = configManager->getFarmConfig("island");
  } catch (const std::exception &e) {
    CCLOG("获取农场配置失败: %s", e.what());
    return;
  }

  // 处理所有对象层
  for (const auto &layerConfig : config.layers.objectLayers) {
    const std::string &layerName = layerConfig.first;
    const auto &layerProps = layerConfig.second;

    try {
      CCLOG("开始处理图层: %s", layerName.c_str());

      // 获取对象组
      auto objectGroup = map->getObjectGroup(layerName);
      if (!objectGroup) {
        CCLOG("未找到对象组: %s", layerName.c_str());
        continue;
      }

      // 检查对象组是否为空
      auto objects = objectGroup->getObjects();
      if (objects.empty()) {
        CCLOG("对象组 %s 中没有对象", layerName.c_str());
        continue;
      }

      // 使用传入的season参数
      
      std::string spritePath =
          SpritePathManager::getIslandSpritePath(layerName, // 层名称
                                               "island",  // 农场类型
                                               season     // 使用传入的季节参数
          );
          

      //std::string spritePath =



      if (spritePath.empty()) {
        CCLOG("未找到对象层 '%s' 的精灵路径", layerName.c_str());
        continue;
      }

      // 验证文件是否存在
      if (!FileUtils::getInstance()->isFileExist(spritePath)) {
        CCLOG("精灵文件不存在: %s", spritePath.c_str());
        continue;
      }

      CCLOG("准备渲染图层 %s", layerName.c_str());
      CCLOG("对象数量: %zu", objects.size());

      LayerRenderer::renderObjectLayer(objectGroup, map, layerName, spritePath,
                                       layerProps);

      CCLOG("图层 %s 处理完成", layerName.c_str());

    } catch (const std::exception &e) {
      CCLOG("处理图层 %s 时发生错误: %s", layerName.c_str(), e.what());
    } catch (...) {
      CCLOG("处理图层 %s 时发生未知错误", layerName.c_str());
    }
  }
}

void FarmMapRenderer::renderStaticObjectLayers(TMXTiledMap *map, const std::string &season) {
  auto configManager = FarmConfigManager::getInstance();
  if (!configManager)
    return;

  try {
    auto config = configManager->getFarmConfig("island");
  } catch (const std::exception &e) {
    CCLOG("渲染静态对象层时发生错误: %s", e.what());
  }
}

void FarmMapRenderer::updateSeason(const std::string &season) {
  // 实现季节更新逻辑
  // 这里可以遍历所有需要更新的对象并更新它们的纹理
}