// InsideConfigManager.h
#ifndef _INSIDE_CONFIG_MANAGER_H_
#define _INSIDE_CONFIG_MANAGER_H_

#include "MapConstants.h"
#include <string>
#include <unordered_map>

// 室内配置结构体
struct InsideConfig {
  std::string tmxPath;
  std::string displayName;
  std::string collisionConfigPath;

  struct Properties {
    int maxFurnitureSlots;
    bool initialUnlocked;
  } properties;

  struct Layers {
    std::unordered_map<std::string, LayerProperties> tileLayers;
    std::unordered_map<std::string, LayerProperties> objectLayers;
  } layers;
};

class InsideConfigManager {
private:
  static InsideConfigManager *instance;
  InsideConfig insideConfig;

  // 加载配置
  void loadConfigs(const std::string &configPath);

public:
  static InsideConfigManager *getInstance();

  // 获取室内配置
  const InsideConfig &getInsideConfig() const;
};

#endif // _INSIDE_CONFIG_MANAGER_H_