// FarmConfigManager.cpp

#include "FarmConfigManager.h"

#include "cocos2d.h"

#include "json/document.h" // RapidJSON

#include "json/filereadstream.h"

using namespace rapidjson;

FarmConfigManager *FarmConfigManager::instance = nullptr;

FarmConfigManager *FarmConfigManager::getInstance() {

  if (instance == nullptr) {

    instance = new FarmConfigManager();

    //CCLOG("创建 FarmConfigManager 实例");

    instance->loadConfigs("configs/maps/farm_types.json"); // 配置文件路径
  }

  return instance;
}

const FarmTypeConfig &
FarmConfigManager::getFarmConfig(const std::string &farmType) const {
  //CCLOG("当前可用的农场类型:");
  for (const auto &[type, config] : farmConfigs) {
    //CCLOG("- %s", type.c_str());
  }

  //CCLOG("正在查找农场类型: %s", farmType.c_str());
  auto it = farmConfigs.find(farmType);

  if (it == farmConfigs.end()) {
    std::string errorMsg = "未找到农场类型 '" + farmType + "' 的配置！请检查配置文件和传入参数是否匹配。";
    //CCLOG("%s", errorMsg.c_str());
    CCASSERT(false, errorMsg.c_str());
  }

  return it->second;
}

std::vector<std::string>

FarmConfigManager::getSeasonEvents(const std::string &farmType,

                                   const std::string &season) const {

  auto it = farmConfigs.find(farmType);

  if (it != farmConfigs.end()) {

    auto seasonIt = it->second.seasonFeatures.find(season);

    if (seasonIt != it->second.seasonFeatures.end()) {

    

      return seasonIt->second.specialEvents;

    } else {

     
    }

  } else {

    //CCLOG("未找到农场类型 '%s' 的配置", farmType.c_str());
  }

  return {};
}

std::vector<std::string>

FarmConfigManager::getAvailableCrops(const std::string &farmType,

                                     const std::string &season) const {

  auto it = farmConfigs.find(farmType);

  if (it != farmConfigs.end()) {

    auto seasonIt = it->second.seasonFeatures.find(season);

    if (seasonIt != it->second.seasonFeatures.end()) {

     
      return seasonIt->second.availableCrops;

    } else {

     
    }

  } else {

    //CCLOG("未找到农场类型 '%s' 的配置", farmType.c_str());
  }

  return {};
}

void FarmConfigManager::loadConfigs(const std::string &configPath) {

  //CCLOG("开始加载配置文件: %s", configPath.c_str());

  std::string fullPath =

      cocos2d::FileUtils::getInstance()->fullPathForFilename(configPath);

  if (fullPath.empty()) {

    //CCLOG("无法找到配置文件: %s", configPath.c_str());

    return;
  }

  FILE *fp = fopen(fullPath.c_str(), "rb");

  if (!fp) {

    //CCLOG("无法打开配置文件: %s", fullPath.c_str());

    return;
  }

  char readBuffer[65536];

  FileReadStream is(fp, readBuffer, sizeof(readBuffer));

  Document doc;

  doc.ParseStream(is);

  fclose(fp);

  if (doc.HasParseError()) {

    //CCLOG("配置文件解析错误: %s (位置: %zu)", configPath.c_str(),

          //doc.GetErrorOffset());

    return;
  }

  if (!doc.IsObject() || !doc.HasMember("farm_types")) {

    //CCLOG("配置文件格式错误或缺少 'farm_types' 字段");

    return;
  }

  const Value &farmTypes = doc["farm_types"];

  //CCLOG("开始解析农场类型配置，数量: %zu", farmTypes.MemberCount());

  for (auto it = farmTypes.MemberBegin(); it != farmTypes.MemberEnd(); ++it) {

    std::string farmType = it->name.GetString();

    //CCLOG("解析农场类型: %s", farmType.c_str());

    const Value &config = it->value;

    FarmTypeConfig farmConfig;

    // 解析 tmx_path

    if (config.HasMember("tmx_path") && config["tmx_path"].IsString()) {

      farmConfig.tmxPath = config["tmx_path"].GetString();

      //CCLOG("  tmxPath: %s", farmConfig.tmxPath.c_str());

    } else {

     //CCLOG("  警告: 'tmx_path' 缺失或不是字符串");
    }

    // 解析 display_name

    if (config.HasMember("display_name") && config["display_name"].IsString()) {

      farmConfig.displayName = config["display_name"].GetString();

      //CCLOG("  displayName: %s", farmConfig.displayName.c_str());

    } else {

      //CCLOG("  警告: 'display_name' 缺失或不是字符串");
    }

    // 解析 collision_config

    if (config.HasMember("collision_config") &&

        config["collision_config"].IsString()) {

      farmConfig.collisionConfigPath = config["collision_config"].GetString();

      //CCLOG("  collisionConfigPath: %s",

            //farmConfig.collisionConfigPath.c_str());

    } else {

      //CCLOG("  警告: 'collision_config' 缺失或不是字符串");
    }

    // 解析 farm_properties

    if (config.HasMember("farm_properties") &&

        config["farm_properties"].IsObject()) {

      const Value &properties = config["farm_properties"];

      if (properties.HasMember("max_crop_slots") &&

          properties["max_crop_slots"].IsInt()) {

        farmConfig.properties.maxCropSlots =

            properties["max_crop_slots"].GetInt();

        //CCLOG("  farm_properties.maxCropSlots: %d",

              //farmConfig.properties.maxCropSlots);

      } else {

        //CCLOG("  警告: 'max_crop_slots' 缺失或不是整数");
      }

      if (properties.HasMember("water_source") &&

          properties["water_source"].IsBool()) {

        farmConfig.properties.hasWaterSource =

            properties["water_source"].GetBool();

        //CCLOG("  farm_properties.hasWaterSource: %d",

              //farmConfig.properties.hasWaterSource);

      } else {

        //CCLOG("  警告: 'water_source' 缺失或不是布尔值");
      }

      if (properties.HasMember("initial_unlocked") &&

          properties["initial_unlocked"].IsBool()) {

        farmConfig.properties.initialUnlocked =

            properties["initial_unlocked"].GetBool();

       /* //CCLOG("  farm_properties.initialUnlocked: %d",

              farmConfig.properties.initialUnlocked);*/

      } else {

       /* //CCLOG("  警告: 'initial_unlocked' 缺失或不是布尔值");*/
      }

    } else {

     // //CCLOG("  警告: 'farm_properties' 缺失或不是对象");
    }

    // 解析 layers

    if (config.HasMember("layers") && config["layers"].IsObject()) {

      const Value &layers = config["layers"];

      // 解析 tile_layers

      if (layers.HasMember("tile_layers") && layers["tile_layers"].IsObject()) {

        const Value &tileLayers = layers["tile_layers"];

       // //CCLOG("  解析 tile_layers，数量: %zu", tileLayers.MemberCount());

        for (auto layerIt = tileLayers.MemberBegin();

             layerIt != tileLayers.MemberEnd(); ++layerIt) {

          std::string layerName = layerIt->name.GetString();

          //CCLOG("    解析 tile_layer: %s", layerName.c_str());

          const Value &props = layerIt->value;

          LayerProperties lp;

          if (props.HasMember("boundary") && props["boundary"].IsBool()) {

            lp.boundary = props["boundary"].GetBool();

            //CCLOG("      boundary: %d", lp.boundary);

          } else {

           // //CCLOG("      警告: 'boundary' 缺失或不是布尔值");
          }

          if (props.HasMember("physical") && props["physical"].IsBool()) {

            lp.physical = props["physical"].GetBool();

           // //CCLOG("      physical: %d", lp.physical);

          } else {

            //CCLOG("      警告: 'physical' 缺失或不是布尔值");
          }

          if (props.HasMember("base") && props["base"].IsBool()) {

            lp.base = props["base"].GetBool();

            //CCLOG("      base: %d", lp.base);

          } else {

            //CCLOG("      警告: 'base' 缺失或不是布尔值");
          }

          if (props.HasMember("penetrable") && props["penetrable"].IsBool()) {

            lp.penetrable = props["penetrable"].GetBool();

            //CCLOG("      penetrable: %d", lp.penetrable);

          } else {

            //CCLOG("      警告: 'penetrable' 缺失或不是布尔值");
          }

          if (props.HasMember("arable") && props["arable"].IsBool()) {
            lp.arable = props["arable"].GetBool();
            //CCLOG("      arable: %d", lp.arable);
          } else {
            lp.arable = false;
            //CCLOG("      arable: 默认为 false");
          }

          if (props.HasMember("fishing") && props["fishing"].IsBool()) {
            lp.fishing = props["fishing"].GetBool();
            //CCLOG("      fishing: %d", lp.fishing);
          } else {
            lp.fishing = false;
            //CCLOG("      fishing: 默认为 false");
          }

          if (props.HasMember("farmpermit") && props["farmpermit"].IsBool()) {
              lp.farmpermit = props["farmpermit"].GetBool();
              //CCLOG("      farmpermit: %d", lp.fishing);
          }
          else {
              lp.farmpermit = false;
              //CCLOG("      farmpermit: 默认为 false");
          }

          farmConfig.layers.tileLayers[layerName] = lp;

          //CCLOG("    已添加 tile_layer: %s", layerName.c_str());
        }

      } else {

        //CCLOG("  警告: 'tile_layers' 缺失或不是对象");
      }

      // 解析 object_layers

      if (layers.HasMember("object_layers") &&

          layers["object_layers"].IsObject()) {

        const Value &objectLayers = layers["object_layers"];

        //CCLOG("  解析 object_layers，数量: %zu", objectLayers.MemberCount());

        for (auto layerIt = objectLayers.MemberBegin();

             layerIt != objectLayers.MemberEnd(); ++layerIt) {

          std::string layerName = layerIt->name.GetString();

          //CCLOG("    解析 object_layer: %s", layerName.c_str());

          const Value &props = layerIt->value;

          LayerProperties lp;

          if (props.HasMember("boundary") && props["boundary"].IsBool()) {

            lp.boundary = props["boundary"].GetBool();

            //CCLOG("      boundary: %d", lp.boundary);

          } else {

            //CCLOG("      警告: 'boundary' 缺失或不是布尔值");
          }

          if (props.HasMember("physical") && props["physical"].IsBool()) {

            lp.physical = props["physical"].GetBool();

            //CCLOG("      physical: %d", lp.physical);

          } else {

            //CCLOG("      警告: 'physical' 缺失或不是布尔值");
          }

          if (props.HasMember("base") && props["base"].IsBool()) {

            lp.base = props["base"].GetBool();

            //CCLOG("      base: %d", lp.base);

          } else {

            //CCLOG("      警告: 'base' 缺失或不是布尔值");
          }

          if (props.HasMember("penetrable") && props["penetrable"].IsBool()) {

            lp.penetrable = props["penetrable"].GetBool();

            //CCLOG("      penetrable: %d", lp.penetrable);

          } else {

            //CCLOG("      警告: 'penetrable' 缺失或不是布尔值");
          }

          farmConfig.layers.objectLayers[layerName] = lp;

          //CCLOG("    已添加 object_layer: %s", layerName.c_str());
        }

      } else {

        //CCLOG("  警告: 'object_layers' 缺失或不是对象");
      }

      // 解析 static_object_layers

      if (layers.HasMember("static_object_layers") &&

          layers["static_object_layers"].IsObject()) {

        const Value &staticObjectLayers = layers["static_object_layers"];

        //CCLOG("  解析 static_object_layers，数量: %zu",

              //staticObjectLayers.MemberCount());

        for (auto layerIt = staticObjectLayers.MemberBegin();

             layerIt != staticObjectLayers.MemberEnd(); ++layerIt) {

          std::string layerName = layerIt->name.GetString();

          //CCLOG("    解析 static_object_layer: %s", layerName.c_str());

          const Value &props = layerIt->value;

          LayerProperties lp;

          if (props.HasMember("boundary") && props["boundary"].IsBool()) {

            lp.boundary = props["boundary"].GetBool();

            //CCLOG("      boundary: %d", lp.boundary);

          } else {

            //CCLOG("      警告: 'boundary' 缺失或不是布尔值");
          }

          if (props.HasMember("physical") && props["physical"].IsBool()) {

            lp.physical = props["physical"].GetBool();

            //CCLOG("      physical: %d", lp.physical);

          } else {

            //CCLOG("      警告: 'physical' 缺失或不是布尔值");
          }

          if (props.HasMember("base") && props["base"].IsBool()) {

            lp.base = props["base"].GetBool();

            //CCLOG("      base: %d", lp.base);

          } else {

            //CCLOG("      警告: 'base' 缺失或不是布尔值");
          }

          if (props.HasMember("penetrable") && props["penetrable"].IsBool()) {

            lp.penetrable = props["penetrable"].GetBool();

            //CCLOG("      penetrable: %d", lp.penetrable);

          } else {

            //CCLOG("      警告: 'penetrable' 缺失或不是布尔值");
          }

          farmConfig.layers.staticObjectLayers[layerName] = lp;

          //CCLOG("    已添加 static_object_layer: %s", layerName.c_str());
        }

      } else {

        //CCLOG("  警告: 'static_object_layers' 缺失或不是对象");
      }

    } else {

      //CCLOG("  警告: 'layers' 缺失或不是对象");
    }

    // 解析 season_features

    if (config.HasMember("season_features") &&

        config["season_features"].IsObject()) {

      const Value &seasons = config["season_features"];

      //CCLOG("  解析 season_features，数量: %zu", seasons.MemberCount());

      for (auto seasonIt = seasons.MemberBegin();

           seasonIt != seasons.MemberEnd(); ++seasonIt) {

        std::string season = seasonIt->name.GetString();

        //CCLOG("    解析季节: %s", season.c_str());

        const Value &features = seasonIt->value;

        FarmTypeConfig::SeasonFeatures sf;

        // 解析 available_crops

        if (features.HasMember("available_crops") &&

            features["available_crops"].IsArray()) {

          const Value &crops = features["available_crops"];

          for (auto cropIt = crops.Begin(); cropIt != crops.End(); ++cropIt) {

            if (cropIt->IsString()) {

              sf.availableCrops.emplace_back(cropIt->GetString());

              //CCLOG("      可种植作物: %s", cropIt->GetString());

            } else {

              //CCLOG("      警告: 'available_crops' 中存在非字符串项");
            }
          }

        } else {

          //CCLOG("      警告: 'available_crops' 缺失或不是数组");
        }

        // 解析 special_events

        if (features.HasMember("special_events") &&

            features["special_events"].IsArray()) {

          const Value &events = features["special_events"];

          for (auto eventIt = events.Begin(); eventIt != events.End();

               ++eventIt) {

            if (eventIt->IsString()) {

              sf.specialEvents.emplace_back(eventIt->GetString());

              //CCLOG("      特殊事件: %s", eventIt->GetString());

            } else {

              //CCLOG("      警告: 'special_events' 中存在非字符串项");
            }
          }

        } else {

          //CCLOG("      警告: 'special_events' 缺失或不是数组");
        }

        farmConfig.seasonFeatures[season] = sf;

        //CCLOG("    已添加季节特性: %s", season.c_str());
      }

    } else {

      //CCLOG("  警告: 'season_features' 缺失或不是对象");
    }

    // 解析 npc_spawn_points

    if (config.HasMember("npc_spawn_points") &&

        config["npc_spawn_points"].IsArray()) {

      const Value &npcs = config["npc_spawn_points"];

      //CCLOG("  解析 npc_spawn_points，数量: %zu", npcs.Size());

      for (auto npcIt = npcs.Begin(); npcIt != npcs.End(); ++npcIt) {

        if (npcIt->IsObject()) {

          FarmTypeConfig::NPCSpawnPoint npc;

          if (npcIt->HasMember("id") && (*npcIt)["id"].IsString()) {

            npc.id = (*npcIt)["id"].GetString();

            //CCLOG("    NPC ID: %s", npc.id.c_str());

          } else {

            //CCLOG("    警告: NPC 缺少 'id' 或 'id' 不是字符串");
          }

          if ((*npcIt).HasMember("position") &&

              (*npcIt)["position"].IsArray() &&

              (*npcIt)["position"].Size() >= 2 &&

              (*npcIt)["position"][0].IsFloat() &&

              (*npcIt)["position"][1].IsFloat()) {

            npc.x = (*npcIt)["position"][0].GetFloat();

            npc.y = (*npcIt)["position"][1].GetFloat();

            //CCLOG("    NPC 位置: (%.2f, %.2f)", npc.x, npc.y);

          } else {

            //CCLOG("    警告: NPC 'position' 缺失、不是数组或数组长度不足");
          }

          farmConfig.npcSpawnPoints.emplace_back(npc);

          //CCLOG("    已添加 NPC: %s", npc.id.c_str());

        } else {

          //CCLOG("    警告: 'npc_spawn_points' 中存在非对象项");
        }
      }

    } else {

      //CCLOG("  警告: 'npc_spawn_points' 缺失或不是数组");
    }

    // 存储配置

    farmConfigs[farmType] = farmConfig;

    //CCLOG("加载农场类型配置完成: %s", farmType.c_str());
  }

  //CCLOG("所有农场类型配置已加载完成，共加载 %zu 个农场类型",

        //farmConfigs.size());
}
