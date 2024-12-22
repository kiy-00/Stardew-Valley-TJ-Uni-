// InsideMapRenderer.cpp
#include "InsideMapRenderer.h"
#include "InsideConfigManager.h"
#include "LayerRenderer.h"
#include "SpritePathManager.h"

static InsideMapRenderer *instance = nullptr;

InsideMapRenderer *InsideMapRenderer::getInstance() {
  if (!instance) {
    instance = new InsideMapRenderer();
  }
  return instance;
}

void InsideMapRenderer::renderMap(TMXTiledMap *map) {
  if (!map) {
    CCLOG("地图未正确加载。");
    return;
  }

  renderTileLayers(map);
  renderObjectLayers(map);
}

void InsideMapRenderer::renderTileLayers(TMXTiledMap *map) {
  // 处理基础图层的渲染
  auto configManager = InsideConfigManager::getInstance();
  if (!configManager)
    return;

  try {
    auto config = configManager->getInsideConfig();
  } catch (const std::exception &e) {
    CCLOG("渲染图块层时发生错误: %s", e.what());
  }
}

void InsideMapRenderer::renderObjectLayers(TMXTiledMap *map) {
  // 获取室内配置
  auto configManager = InsideConfigManager::getInstance();
  if (!configManager) {
    CCLOG("InsideConfigManager实例获取失败");
    return;
  }

  InsideConfig config;
  try {
    config = configManager->getInsideConfig();
  } catch (const std::exception &e) {
    CCLOG("获取室内配置失败: %s", e.what());
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

      // 使用新的接口获取精灵路径
      std::string spritePath =
          SpritePathManager::getInsideSpritePath(layerName); // 简化的路径获取

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