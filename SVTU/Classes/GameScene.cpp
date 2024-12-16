#include "GameScene.h"
#include "LayerRenderer.h"
#include "LayerRendererFactory.h"

USING_NS_CC;

// 创建场景
Scene *GameScene::createScene() { return GameScene::create(); }

bool GameScene::init() {
  if (!Scene::init()) {
    return false;
  }

  // 初始化地图
  if (!initTileMap()) {
    return false;
  }
  CCLOG("地图已加载");

  // 初始化玩家
  if (!initPlayer()) {
    return false;
  }

  // 获取农场配置
  FarmTypeConfig islandConfig =
      FarmConfigManager::getInstance()->getFarmConfig("island");

  // 对父容器进行缩放（例如放大两倍）
  float scaleFactor = 2.0f;
  mapContainer->setScale(scaleFactor);
  mapContainer->setPosition(Vec2(0, 0));
  CCLOG("已对地图容器进行缩放，缩放比例: %.2f", scaleFactor);

  // 设置相机
  setupCamera();

  // 设置键盘输入
  setupKeyboard();

  return true;
}

// GameScene.cpp

bool GameScene::initTileMap() {
  // 创建父容器节点
  mapContainer = cocos2d::Node::create();
  this->addChild(mapContainer);

  // 加载瓦片地图
  tmxMap = cocos2d::TMXTiledMap::create("maps/farm/island/island_spring.tmx");
  if (tmxMap) {
    // 设置锚点和位置
    tmxMap->setAnchorPoint(cocos2d::Vec2(0, 0));
    tmxMap->setPosition(cocos2d::Vec2(0, 0));
    baseLayer = tmxMap->getLayer("base");

    // 将地图添加到父容器
    mapContainer->addChild(tmxMap);
    // this->addChild(tmxMap);
    tmxMap->setName("tmxMap");

    // 获取地图和图块尺寸（可选，用于调试）
    cocos2d::Size mapSizeInTiles = tmxMap->getMapSize();
    cocos2d::Size tileSize = tmxMap->getTileSize();
    float mapPixelWidth = mapSizeInTiles.width * tileSize.width;
    float mapPixelHeight = mapSizeInTiles.height * tileSize.height;

    // 绘制地图边界（可选，用于调试）
    auto border = cocos2d::DrawNode::create();
    cocos2d::Vec2 vertices[4] = {cocos2d::Vec2(0, 0),
                                 cocos2d::Vec2(mapPixelWidth, 0),
                                 cocos2d::Vec2(mapPixelWidth, mapPixelHeight),
                                 cocos2d::Vec2(0, mapPixelHeight)};
    border->drawPoly(vertices, 4, true, cocos2d::Color4F::GREEN);
    tmxMap->addChild(border, 100); // 确保边框在最上层

    // 加载房屋精灵
    // loadHouseSprites();

    loadObjectLayers();

    return true;
  } else {
    CCLOG("加载地图失败，当前工作目录: %s", cocos2d::FileUtils::getInstance()
                                                ->getDefaultResourceRootPath()
                                                .c_str());
    return false;
  }
}

bool GameScene::initPlayer() {
  // 创建玩家精灵
  player = cocos2d::Sprite::create("HelloWorld.png");
  player->setScale(0.2f); // 设置缩放比例
  if (player) {
    // CCLOG("initPlayer: 玩家精灵已创建，名称: %s", player->getName().c_str());
    // 设置锚点为左下角
    player->setAnchorPoint(cocos2d::Vec2(0, 0));

    // 设置玩家初始位置为地图中心
    player->setPosition(cocos2d::Vec2(1600, 1600));
    // 创建一个动态的物理体积
    auto physicsBody = cocos2d::PhysicsBody::createBox(
        player->getContentSize(),
        cocos2d::PhysicsMaterial(1.0f, 0.5f, 0.5f) // 根据需求调整物理材料参数
    );
    physicsBody->setDynamic(false);        // 动态物体
    physicsBody->setCategoryBitmask(0x01); // 类别掩码
    // physicsBody->setCollisionBitmask(0x02);         // 与房屋碰撞
    physicsBody->setCollisionBitmask(0xFFFFFFF);    // 与房屋碰撞
    physicsBody->setContactTestBitmask(0xFFFFFFFF); // 接触测试掩码

    // CCLOG("玩家已添加物理体积。");

    // 将物理体积添加到玩家
    // this->addComponent(physicsBody);
    player->setPhysicsBody(physicsBody);

    if (player->getPhysicsBody()) {
      CCLOG("玩家精灵已成功添加物理体积。");
    } else {
      CCLOG("玩家精灵未能添加物理体积！");
    }

    // 将玩家添加到父容器
    mapContainer->addChild(player);
    // this->addChild(player);
    // tmxMap->addChild(player, 1000);
    // CCLOG("玩家精灵已创建并添加到场景，初始位置: (1600, 1600)");

    // 验证玩家位置是否在地图范围内
    cocos2d::Size mapSizeInTiles = tmxMap->getMapSize();
    cocos2d::Size tileSize = tmxMap->getTileSize();
    float mapPixelWidth = mapSizeInTiles.width * tileSize.width;
    float mapPixelHeight = mapSizeInTiles.height * tileSize.height;
    cocos2d::Vec2 playerPos = player->getPosition();
    // CCLOG("玩家位置验证: x = %.2f, y = %.2f", playerPos.x, playerPos.y);
    if (playerPos.x < 0 || playerPos.x > mapPixelWidth || playerPos.y < 0 ||
        playerPos.y > mapPixelHeight) {
      CCLOG("警告: 玩家初始位置超出地图范围!");
    }

    // 获取精灵的原始尺寸
    cocos2d::Size spriteSize = player->getContentSize();

    // 添加调试边框（可选）
    auto debugRect = cocos2d::DrawNode::create();
    cocos2d::Vec2 vertices[4] = {
        cocos2d::Vec2(0, 0), cocos2d::Vec2(spriteSize.width, 0),
        cocos2d::Vec2(spriteSize.width, spriteSize.height),
        cocos2d::Vec2(0, spriteSize.height)};
    debugRect->drawRect(vertices[0], vertices[1], vertices[2], vertices[3],
                        cocos2d::Color4F::BLUE);
    player->addChild(debugRect);
    // CCLOG("已为玩家添加调试边框");

    return true;
  } else {
    CCLOG("创建玩家精灵失败");
    return false;
  }
}

void GameScene::setupCamera() {
  // 获取默认摄像机
  auto camera = this->getDefaultCamera();

  // 设置摄像机的初始位置为玩家的位置
  camera->setPosition(player->getPosition());

  // 启用每帧更新
  this->scheduleUpdate();
}
void GameScene::movePlayerRandomly(float dt) {
  if (!player || !tmxMap)
    return;

  Size mapSizeInTiles = tmxMap->getMapSize();
  Size tileSize = tmxMap->getTileSize();
  float mapPixelWidth = mapSizeInTiles.width * tileSize.width;
  float mapPixelHeight = mapSizeInTiles.height * tileSize.height;

  // 生成随机坐标，确保玩家不会移动到地图外
  float randomX =
      static_cast<float>(std::rand() % static_cast<int>(mapPixelWidth));
  float randomY =
      static_cast<float>(std::rand() % static_cast<int>(mapPixelHeight));
  Vec2 targetPos(randomX, randomY);

  // // 检查目标位置是否可通过
  // if (isPositionWalkable(targetPos)) {
  //   CCLOG("目标位置不可通过: (%.2f, %.2f)", randomX, randomY);
  //   return; // 取消移动
  // }

  CCLOG("随机移动玩家到: (%.2f, %.2f)", randomX, randomY);

  // 创建移动动作
  auto moveAction = MoveTo::create(2.0f, targetPos);

  // 创建一个回调函数，移动完成后输出玩家的新位置
  auto callback = CallFunc::create([this]() {
    Vec2 newPos = player->getPosition();
    // CCLOG("玩家已移动到: (%.2f, %.2f)", newPos.x, newPos.y);
  });

  // 创建序列动作
  auto sequence = Sequence::create(moveAction, callback, nullptr);
  player->runAction(sequence);
}

void GameScene::createHouseSprite(TMXTiledMap *map, int gid, float x, float y,
                                  float width, float height) {
  // 假设 gid=5 对应 "spring.png"
  std::string spriteFile = "maps/farm/island/asset/house/house_1/spring.png";

  // 创建精灵
  auto sprite = Sprite::create(spriteFile);
  if (sprite) {
    // CCLOG("创建房屋精灵成功，文件: %s", spriteFile.c_str());
    // 设置锚点为左下角
    sprite->setAnchorPoint(Vec2(0, 0));

    // 获取精灵的原始尺寸
    Size spriteSize = sprite->getContentSize();

    // 获取地图尺寸和图块尺寸
    Size mapSizeInTiles = map->getMapSize();
    Size tileSize = map->getTileSize();
    float mapPixelWidth = mapSizeInTiles.width * tileSize.width;
    float mapPixelHeight = mapSizeInTiles.height * tileSize.height;

    // 打印调试信息
    // CCLOG("地图大小（像素）: %.2f x %.2f", mapPixelWidth, mapPixelHeight);
    // CCLOG("房屋原始坐标: x = %.2f, y = %.2f", x, y);
    // CCLOG("对象大小: width = %.2f, height = %.2f", width, height);
    // CCLOG("精灵原始大小: width = %.2f, height = %.2f", spriteSize.width,
    //       spriteSize.height);

    // 计算缩放比例以匹配对象的尺寸
    float scaleX = (width > 0) ? width / spriteSize.width : 1.0f;
    float scaleY = (height > 0) ? height / spriteSize.height : 1.0f;
    sprite->setScale(scaleX, scaleY);
    // CCLOG("精灵缩放比例: scaleX = %.2f, scaleY = %.2f", scaleX, scaleY);

    // 计算对象的正确位置
    // Cocos2d-x 的 y 坐标需要进行转换
    float correctedY = y;
    float correctedX = x;

    // CCLOG("计算后的房屋位置: (%.2f, %.2f)", correctedX, correctedY);

    // 设置精灵的位置
    sprite->setPosition(Vec2(correctedX, correctedY));

    // 设置 Z 顺序，确保房屋在基础层之上
    sprite->setLocalZOrder(1);
    // CCLOG("设置精灵位置和 Z 顺序");

    // 将精灵添加到地图节点
    map->addChild(sprite);
    // CCLOG("已创建并添加房屋精灵，GID: %d, 位置: (%.2f, %.2f)", gid,
    // correctedX,
    //       correctedY);

    if (sprite->getPhysicsBody()) {
      CCLOG("房屋精灵已成功添加物理体积。");
    } else {
      CCLOG("房屋精灵未能添加物理体积！");
    }

    // 添加调试边框（可选）
    auto debugRect = DrawNode::create();
    Vec2 vertices[4] = {Vec2(0, 0), Vec2(width, 0), Vec2(width, height),
                        Vec2(0, height)};
    debugRect->drawRect(vertices[0], vertices[1], vertices[2], vertices[3],
                        Color4F::RED);
    sprite->addChild(debugRect);
    // CCLOG("已添加调试边框到房屋精灵");

    // 添加位置标记（可选）
    auto marker = DrawNode::create();
    marker->drawDot(Vec2(correctedX, correctedY), 5, Color4F::BLUE);
    map->addChild(marker, 200); // 确保标记在房屋精灵之上
    // CCLOG("已添加位置标记到房屋精灵");
  } else {
    CCLOG("无法创建精灵，文件: %s", spriteFile.c_str());
  }
}

void GameScene::update(float dt) {
  // 调用父类的更新方法
  Scene::update(dt);

  // 获取默认摄像机
  auto camera = this->getDefaultCamera();

  // 获取地图尺寸
  cocos2d::Size mapSizeInTiles = tmxMap->getMapSize();
  cocos2d::Size tileSize = tmxMap->getTileSize();
  float mapWidth =
      mapSizeInTiles.width * tileSize.width * mapContainer->getScale();
  float mapHeight =
      mapSizeInTiles.height * tileSize.height * mapContainer->getScale();

  // 获取可视区域尺寸
  cocos2d::Size visibleSize =
      cocos2d::Director::getInstance()->getVisibleSize();
  float halfWidth = visibleSize.width / 2;
  float halfHeight = visibleSize.height / 2;

  // 获取玩家位置并考虑缩放
  cocos2d::Vec2 playerPos = player->getPosition() * mapContainer->getScale();

  // 限制摄像机位置，使其不超出地图边界
  float minX = halfWidth;
  float maxX = mapWidth - halfWidth;
  float minY = halfHeight;
  float maxY = mapHeight - halfHeight;

  float posX = playerPos.x;
  float posY = playerPos.y;

  // 防止摄像机超出左、下边界
  if (posX < minX) {
    posX = minX;
  }
  if (posY < minY) {
    posY = minY;
  }

  // 防止摄像机超出右、上边界
  if (posX > maxX) {
    posX = maxX;
  }
  if (posY > maxY) {
    posY = maxY;
  }

  // 设置摄像机位置
  camera->setPosition(cocos2d::Vec2(posX, posY));
}
// 自定义绘制方法
void GameScene::draw(Renderer *renderer, const Mat4 &transform,
                     uint32_t flags) {
  Scene::draw(renderer, transform, flags);
  // 其他自定义渲染逻辑
}

// void GameScene::loadHouseSprites() {
//   auto map = this->tmxMap;
//   if (!map) {
//     CCLOG("地图未正确加载。");
//     return;
//   }

//   auto houseGroup = map->getObjectGroup("house");
//   if (!houseGroup) {
//     CCLOG("未找到名为 'house' 的对象组。");
//     return;
//   }

//   // 获取地图尺寸信息用于调试
//   Size mapSizeInTiles = map->getMapSize();
//   Size tileSize = map->getTileSize();
//   float mapPixelWidth = mapSizeInTiles.width * tileSize.width;
//   float mapPixelHeight = mapSizeInTiles.height * tileSize.height;
//   CCLOG("地图信息 - 瓦片数: %.0f x %.0f, 瓦片大小: %.0f x %.0f, 总像素: %.0f
//   x "
//         "%.0f",
//         mapSizeInTiles.width, mapSizeInTiles.height, tileSize.width,
//         tileSize.height, mapPixelWidth, mapPixelHeight);

//   const auto objects = houseGroup->getObjects();
//   CCLOG("找到房屋对象数量: %zu", objects.size());

//   for (const auto &obj : objects) {
//     ValueMap object = obj.asValueMap();

//     CCLOG("===== TMX文件解析过程 =====");
//     // 打印完整的对象数据
//     for (const auto &pair : object) {
//       // 根据Value的类型打印值
//       std::string valueStr;
//       switch (pair.second.getType()) {
//       case Value::Type::INTEGER:
//         valueStr = std::to_string(pair.second.asInt());
//         break;
//       case Value::Type::FLOAT:
//         valueStr = std::to_string(pair.second.asFloat());
//         break;
//       case Value::Type::STRING:
//         valueStr = pair.second.asString();
//         break;
//       default:
//         valueStr = "未知类型";
//         break;
//       }
//       CCLOG("键: %s, 值类型: %d, 值: %s", pair.first.c_str(),
//             static_cast<int>(pair.second.getType()), valueStr.c_str());
//     }

//     if (object.size() > 0) {
//       // 打印原始TMX数据
//       CCLOG("===== 房屋对象数据 =====");
//       if (object.find("id") != object.end()) {
//         CCLOG("对象ID: %d", object["id"].asInt());
//       } else {
//         CCLOG("对象ID: 不存在");
//       }

//       if (object.find("gid") != object.end()) {
//         CCLOG("GID: %d", object["gid"].asInt());
//       } else {
//         CCLOG("GID: 不存在");
//       }

//       if (object.find("x") != object.end() &&
//           object.find("y") != object.end()) {
//         CCLOG("TMX原始坐标: x=%.2f, y=%.2f", object["x"].asFloat(),
//               object["y"].asFloat());
//       } else {
//         CCLOG("TMX原始坐标: x 或 y 不存在");
//       }

//       if (object.find("width") != object.end() &&
//           object.find("height") != object.end()) {
//         CCLOG("尺寸: width=%.2f, height=%.2f", object["width"].asFloat(),
//               object["height"].asFloat());
//       } else {
//         CCLOG("尺寸: width 或 height 不存在");
//       }

//       // 获取数据，添加存在性检查
//       int gid = object.find("gid") != object.end() ? object["gid"].asInt() :
//       0; float x = object.find("x") != object.end() ? object["x"].asFloat() :
//       0.0f; float y = object.find("y") != object.end() ?
//       object["y"].asFloat() : 0.0f; float width = object.find("width") !=
//       object.end()
//                         ? object["width"].asFloat()
//                         : 0.0f;
//       float height = object.find("height") != object.end()
//                          ? object["height"].asFloat()
//                          : 0.0f;

//       // 验证数据转换
//       CCLOG("传递给createHouseSprite的数据:");
//       CCLOG("位置: (%.2f, %.2f), 尺寸: %.2f x %.2f", x, y, width, height);
//       CCLOG("====================");

//       // 创建房屋精灵
//       createHouseSprite(map, gid, x, y, width, height);
//     }
//   }
// }

// void GameScene::applyLayerProperties(const FarmTypeConfig &config) {
//   // 应用 tile_layers
//   applyLayers(config.layers.tileLayers, "tile_layer");

//   // 应用 object_layers
//   applyLayers(config.layers.objectLayers, "object_layer");

//   // 应用 static_object_layers
//   applyLayers(config.layers.staticObjectLayers, "static_object_layer");
// }

// void GameScene::applyLayers(
//     const std::unordered_map<std::string, LayerProperties> &layers,
//     const std::string &layerType) {
//   for (const auto &pair : layers) {
//     const std::string &layerName = pair.first;
//     const LayerProperties &props = pair.second;

//     TMXLayer *layer = tmxMap->getLayer(layerName);
//     if (layer) {
//       layer->setLocalZOrder(props.base ? 1 : 0);
//       CCLOG("设置层 %s (%s) 的属性: boundary=%d, physical=%d, base=%d, "
//             "penetrable=%d",
//             layerName.c_str(), layerType.c_str(), props.boundary,
//             props.physical, props.base, props.penetrable);

//       // 移除碰撞处理
//       /*
//       if (props.boundary) {
//           addWaterCollision(layer);
//       }
//       if (props.physical) {
//           addBaseCollision(layer);
//       }
//       */
//     } else {
//       // CCLOG("未找到 %s 层: %s", layerType.c_str(), layerName.c_str());
//     }
//   }
// }
// void GameScene::addBoundaryToLayer(TMXLayer *layer) {
//   CCLOG("添加边界碰撞到层: %s", layer->getLayerName().c_str());

//   // 获取地图尺寸和图块尺寸
//   Size mapSize = tmxMap->getMapSize();
//   Size tileSize = tmxMap->getTileSize();

//   // 遍历每个图块，添加边界碰撞
//   for (int x = 0; x < mapSize.width; ++x) {
//     for (int y = 0; y < mapSize.height; ++y) {
//       int gid = layer->getTileGIDAt(cocos2d::Vec2(x, y));
//       if (gid != 0) { // 有图块
//                       // 获取图块属性（如果有）

//         // 计算图块中心位置
//         float posX = x * tileSize.width + tileSize.width / 2;
//         float posY = y * tileSize.height + tileSize.height / 2;

//         // 创建物理体积
//         auto physicsBody =
//             PhysicsBody::createBox(tileSize, PHYSICSBODY_MATERIAL_DEFAULT);
//         physicsBody->setDynamic(false);
//         physicsBody->setCategoryBitmask(0x02);  // boundary 分类
//         physicsBody->setCollisionBitmask(0x02); // boundary 碰撞
//         physicsBody->setContactTestBitmask(0xFFFFFFFF);

//         // 创建一个节点用于物理体积
//         auto boundaryNode = cocos2d::Node::create();
//         boundaryNode->setPosition(cocos2d::Vec2(posX, posY));
//         boundaryNode->setPhysicsBody(physicsBody);
//         //mapContainer->addChild(boundaryNode);

//         CCLOG("添加边界碰撞体 at (%.2f, %.2f)", posX, posY);
//       }
//     }
//   }
// }

void GameScene::setupKeyboard() {
  auto keyboardListener = EventListenerKeyboard::create();

  keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);

  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener,
                                                           this);
}

void GameScene::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode,
                             cocos2d::Event *event) {
  float moveDistance = 32.0f; // 每次移动的像素数

  Vec2 currentPos = player->getPosition();
  Vec2 targetPos = currentPos;

  switch (keyCode) {
  case EventKeyboard::KeyCode::KEY_W:
    targetPos.y += moveDistance;
    break;
  case EventKeyboard::KeyCode::KEY_S:
    targetPos.y -= moveDistance;
    break;
  case EventKeyboard::KeyCode::KEY_A:
    targetPos.x -= moveDistance;
    break;
  case EventKeyboard::KeyCode::KEY_D:
    targetPos.x += moveDistance;
    break;
  default:
    return; // 不处理其他键
  }

  // // 检查目标位置是否可通过
  // if (!isPositionWalkable(targetPos)) {
  //   CCLOG("目标位置不可通过: (%.2f, %.2f)", targetPos.x, targetPos.y);
  //   return;
  // }

  // CCLOG("移动玩家到: (%.2f, %.2f)", targetPos.x, targetPos.y);

  // 创建移动动作
  auto moveAction = MoveTo::create(0.2f, targetPos);

  // 创建一个回调函数，移动完成后输出玩家的新位置
  auto callback = CallFunc::create([this]() {
    Vec2 newPos = player->getPosition();
    // CCLOG("玩家已移动到: (%.2f, %.2f)", newPos.x, newPos.y);
  });

  // 创建序列动作
  auto sequence = Sequence::create(moveAction, callback, nullptr);
  player->runAction(sequence);
}

void GameScene::addPhysicalToLayer(TMXLayer *layer) {
  CCLOG("添加物理碰撞到层: %s", layer->getLayerName().c_str());

  // // 遍历每个图块，添加物理体积
  // Size mapSize = layer->getMapSize();
  // Size tileSize = layer->getTileSize();

  // for (int x = 0; x < mapSize.width; ++x) {
  //   for (int y = 0; y < mapSize.height; ++y) {
  //     int gid = layer->getTileGIDAt(Vec2(x, y));
  //     if (gid != 0) { // 有图块
  //       // 获取图块属性（如果有）
  //       // 可以根据需要从TMX文件中读取更多信息

  //       // 创建物理体积
  //       auto tilePos = Vec2(x * tileSize.width + tileSize.width / 2,
  //                           y * tileSize.height + tileSize.height / 2);
  //       auto physicsBody =
  //           PhysicsBody::createBox(tileSize, PHYSICSBODY_MATERIAL_DEFAULT);
  //       physicsBody->setDynamic(false);
  //       physicsBody->setCategoryBitmask(0x02);
  //       physicsBody->setCollisionBitmask(0x02);
  //       physicsBody->setContactTestBitmask(0xFFFFFFFF);

  //       // 创建一个精灵用于物理体积
  //       auto physicsNode = Node::create();
  //       physicsNode->setPosition(tilePos);
  //       physicsNode->setPhysicsBody(physicsBody);
  //       mapContainer->addChild(physicsNode);
  //     }
  //   }
  // }
}

// // 修改 isPositionWalkable 方法，修正逻辑，只有当 baseLayer 有瓦片时才可行走
// bool GameScene::isPositionWalkable(const cocos2d::Vec2 &position) {
//   // 获取 mapContainer 的缩放比例（仅用于日志，不用于计算）
//   float scale = mapContainer->getScale();

//   // 由于 mapContainer 已经缩放，传入的 position 是逻辑坐标，不需要再次缩放
//   cocos2d::Vec2 logicalPos = position;
//   // CCLOG("scale: %.2f, logicalPos: (%.2f, %.2f)", scale, logicalPos.x,
//   //       logicalPos.y);

//   // 获取瓦片尺寸和地图尺寸
//   cocos2d::Size tileSize = baseLayer->getMapTileSize();
//   cocos2d::Size mapSizeInTiles = baseLayer->getLayerSize();
//   float mapPixelWidth = mapSizeInTiles.width * tileSize.width;
//   float mapPixelHeight = mapSizeInTiles.height * tileSize.height;

//   // Cocos2d-x 的坐标系统原点在左下角，TMX 的原点在左上角，需要翻转 y 坐标
//   float flippedY = mapPixelHeight - logicalPos.y;

//   // 将像素坐标转换为瓦片坐标
//   int x = static_cast<int>(logicalPos.x / tileSize.width);
//   int y = static_cast<int>(flippedY / tileSize.height);

//   // CCLOG("检查位置是否可通过: (logicalPos.x=%.2f, logicalPos.y=%.2f) ->
//   (x=%d,
//   // y=%d)",
//   //       logicalPos.x, logicalPos.y, x, y);

//   // 边界检查
//   if (x < 0 || x >= mapSizeInTiles.width || y < 0 ||
//       y >= mapSizeInTiles.height) {
//     // CCLOG("位置超出地图范围: (x=%d, y=%d)", x, y);
//     return false; // 超出地图范围，不可行走
//   }

//   // 获取瓦片 GID
//   int gid = baseLayer->getTileGIDAt(cocos2d::Vec2(x, y));
//   // CCLOG("瓦片 GID at (x=%d, y=%d): %d", x, y, gid);

//   // 修正逻辑：当 baseLayer 有瓦片时（gid != 0），表示可通过
//   bool isWalkable = (gid != 0);
//   return isWalkable;
// }

void GameScene::loadObjectLayers() {
  auto map = this->tmxMap;
  if (!map) {
    CCLOG("地图未正确加载。");
    return;
  }

  std::vector<std::string> objectLayers = {"house",    "myhouse",  "obj_fb_2",
                                           "obj_mv_1", "static_1", "static_4",
                                           "static_5"};

  // 获取农场配置
  FarmTypeConfig islandConfig =
      FarmConfigManager::getInstance()->getFarmConfig("island");
  for (const auto &layerName : objectLayers) {
    std::string spritePath = LayerRendererFactory::getSpritePath(layerName);
    auto objectGroup = map->getObjectGroup(layerName);
    if (!spritePath.empty()) {
      LayerRenderer::renderObjectLayer(
          objectGroup, map, layerName, spritePath,
          islandConfig.layers.objectLayers[layerName]);
    } else {
      CCLOG("未找到对象层 '%s' 的精灵路径。", layerName.c_str());
    }
  }
}