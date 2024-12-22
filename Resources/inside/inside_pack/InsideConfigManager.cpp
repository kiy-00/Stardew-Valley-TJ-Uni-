// InsideConfigManager.cpp
#include "InsideConfigManager.h"
#include "cocos2d.h"
#include "json/document.h"
#include "json/filereadstream.h"

using namespace rapidjson;

InsideConfigManager *InsideConfigManager::instance = nullptr;

InsideConfigManager *InsideConfigManager::getInstance() {
  if (instance == nullptr) {
    instance = new InsideConfigManager();
    CCLOG("创建 InsideConfigManager 实例");
    instance->loadConfigs("configs/maps/inside.json");
  }
  return instance;
}

const InsideConfig &InsideConfigManager::getInsideConfig() const {
  return insideConfig;
}

void InsideConfigManager::loadConfigs(const std::string &configPath) {
  CCLOG("开始加载配置文件: %s", configPath.c_str());

  std::string fullPath =
      cocos2d::FileUtils::getInstance()->fullPathForFilename(configPath);
  if (fullPath.empty()) {
    CCLOG("无法找到配置文件: %s", configPath.c_str());
    return;
  }

  FILE *fp = fopen(fullPath.c_str(), "rb");
  if (!fp) {
    CCLOG("无法打开配置文件: %s", fullPath.c_str());
    return;
  }

  char readBuffer[65536];
  FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  Document doc;
  doc.ParseStream(is);
  fclose(fp);

  if (doc.HasParseError()) {
    CCLOG("配置文件解析错误: %s (位置: %zu)", configPath.c_str(),
          doc.GetErrorOffset());
    return;
  }

  if (!doc.IsObject() || !doc.HasMember("inside")) {
    CCLOG("配置文件格式错误或缺少 'inside' 字段");
    return;
  }

  const Value &config = doc["inside"];
  CCLOG("开始解析室内配置");

  // 解析基本属性
  if (config.HasMember("tmx_path") && config["tmx_path"].IsString()) {
    insideConfig.tmxPath = config["tmx_path"].GetString();
    CCLOG("  tmxPath: %s", insideConfig.tmxPath.c_str());
  }

  if (config.HasMember("display_name") && config["display_name"].IsString()) {
    insideConfig.displayName = config["display_name"].GetString();
    CCLOG("  displayName: %s", insideConfig.displayName.c_str());
  }

  if (config.HasMember("collision_config") &&
      config["collision_config"].IsString()) {
    insideConfig.collisionConfigPath = config["collision_config"].GetString();
    CCLOG("  collisionConfigPath: %s",
          insideConfig.collisionConfigPath.c_str());
  }

  // 解析室内属性
  if (config.HasMember("inside_properties") &&
      config["inside_properties"].IsObject()) {
    const Value &properties = config["inside_properties"];

    if (properties.HasMember("max_furniture_slots") &&
        properties["max_furniture_slots"].IsInt()) {
      insideConfig.properties.maxFurnitureSlots =
          properties["max_furniture_slots"].GetInt();
      CCLOG("  inside_properties.maxFurnitureSlots: %d",
            insideConfig.properties.maxFurnitureSlots);
    }

    if (properties.HasMember("initial_unlocked") &&
        properties["initial_unlocked"].IsBool()) {
      insideConfig.properties.initialUnlocked =
          properties["initial_unlocked"].GetBool();
      CCLOG("  inside_properties.initialUnlocked: %d",
            insideConfig.properties.initialUnlocked);
    }
  }

  // 解析图层
  if (config.HasMember("layers") && config["layers"].IsObject()) {
    const Value &layers = config["layers"];

    // 解析 tile_layers
    if (layers.HasMember("tile_layers") && layers["tile_layers"].IsObject()) {
      const Value &tileLayers = layers["tile_layers"];
      CCLOG("  解析 tile_layers，数量: %zu", tileLayers.MemberCount());

      for (auto layerIt = tileLayers.MemberBegin();
           layerIt != tileLayers.MemberEnd(); ++layerIt) {
        std::string layerName = layerIt->name.GetString();
        CCLOG("    解析 tile_layer: %s", layerName.c_str());

        const Value &props = layerIt->value;
        LayerProperties lp;

        if (props.HasMember("boundary") && props["boundary"].IsBool()) {
          lp.boundary = props["boundary"].GetBool();
        }
        if (props.HasMember("physical") && props["physical"].IsBool()) {
          lp.physical = props["physical"].GetBool();
        }
        if (props.HasMember("base") && props["base"].IsBool()) {
          lp.base = props["base"].GetBool();
        }
        if (props.HasMember("penetrable") && props["penetrable"].IsBool()) {
          lp.penetrable = props["penetrable"].GetBool();
        }

        insideConfig.layers.tileLayers[layerName] = lp;
      }
    }

    // 解析 object_layers
    if (layers.HasMember("object_layers") &&
        layers["object_layers"].IsObject()) {
      const Value &objectLayers = layers["object_layers"];
      CCLOG("  解析 object_layers，数量: %zu", objectLayers.MemberCount());

      for (auto layerIt = objectLayers.MemberBegin();
           layerIt != objectLayers.MemberEnd(); ++layerIt) {
        std::string layerName = layerIt->name.GetString();
        CCLOG("    解析 object_layer: %s", layerName.c_str());

        const Value &props = layerIt->value;
        LayerProperties lp;

        if (props.HasMember("boundary") && props["boundary"].IsBool()) {
          lp.boundary = props["boundary"].GetBool();
        }
        if (props.HasMember("physical") && props["physical"].IsBool()) {
          lp.physical = props["physical"].GetBool();
        }
        if (props.HasMember("base") && props["base"].IsBool()) {
          lp.base = props["base"].GetBool();
        }
        if (props.HasMember("penetrable") && props["penetrable"].IsBool()) {
          lp.penetrable = props["penetrable"].GetBool();
        }

        insideConfig.layers.objectLayers[layerName] = lp;
      }
    }
  }

  CCLOG("室内配置加载完成");
}