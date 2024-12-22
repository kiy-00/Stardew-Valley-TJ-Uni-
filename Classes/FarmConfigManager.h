// FarmConfigManager.h
#ifndef _FARM_CONFIG_MANAGER_H_

#define _FARM_CONFIG_MANAGER_H_

#include "MapConstants.h"

#include <string>

#include <unordered_map>

#include <vector>

// 层属性结构体

// 农场类型配置

struct FarmTypeConfig {

  std::string tmxPath;

  std::string displayName;

  std::string collisionConfigPath;

  struct Properties {

    int maxCropSlots;

    bool hasWaterSource;

    bool initialUnlocked;

  } properties;

  struct SeasonFeatures {

    std::vector<std::string> availableCrops;

    std::vector<std::string> specialEvents;
  };

  std::unordered_map<std::string, SeasonFeatures> seasonFeatures;

  struct NPCSpawnPoint {

    std::string id;

    float x;

    float y;
  };

  std::vector<NPCSpawnPoint> npcSpawnPoints;

  struct Layers {

    std::unordered_map<std::string, LayerProperties> tileLayers;

    std::unordered_map<std::string, LayerProperties> objectLayers;

    std::unordered_map<std::string, LayerProperties> staticObjectLayers;

  } layers;
};

class FarmConfigManager {

private:
  static FarmConfigManager *instance;

  std::unordered_map<std::string, FarmTypeConfig> farmConfigs;

  // 加载所有配置

  void loadConfigs(const std::string &configPath);

public:
  static FarmConfigManager *getInstance();

  // 获取特定农场配置

  const FarmTypeConfig &getFarmConfig(const std::string &farmType) const;

  // 检查特定农场在当前季节是否有特殊事件

  std::vector<std::string> getSeasonEvents(const std::string &farmType,

                                           const std::string &season) const;

  // 获取当前季节可种植作物

  std::vector<std::string> getAvailableCrops(const std::string &farmType,

                                             const std::string &season) const;
};

#endif // _FARM_CONFIG_MANAGER_H_