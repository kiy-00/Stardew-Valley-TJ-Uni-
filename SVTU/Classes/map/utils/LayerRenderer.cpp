// LayerRenderer.cpp
#include "LayerRenderer.h"
#include "GameObject.h"

// 渲染普通图层
void LayerRenderer::renderLayer(cocos2d::TMXLayer *layer,
                                const std::string &layerName,
                                const std::string &spritePath,
                                const LayerProperties &layerProps) {

  CCLOG("成功渲染普通图层: %s, 精灵路径: %s", layerName.c_str(),
        spritePath.c_str());
}

// 渲染对象层
void LayerRenderer::renderObjectLayer(cocos2d::TMXObjectGroup *objectGroup,
                                      cocos2d::TMXTiledMap *tmxMap,
                                      const std::string &layerName,
                                      const std::string &spritePath,
                                      const LayerProperties &layerProps) {
  if (!objectGroup) {
    CCLOG("未找到名为 '%s' 的对象组。", layerName.c_str());
    return;
  }

  const auto &objects = objectGroup->getObjects();
  CCLOG("找到 %s 对象数量: %zu", layerName.c_str(), objects.size());

  for (const auto &obj : objects) {
    cocos2d::ValueMap object = obj.asValueMap();

    // 打印调试信息
    // CCLOG("===== TMX文件解析过程 =====");
    // for (const auto &pair : object) {
    //   std::string valueStr;
    //   switch (pair.second.getType()) {
    //   case cocos2d::Value::Type::INTEGER:
    //     valueStr = std::to_string(pair.second.asInt());
    //     break;
    //   case cocos2d::Value::Type::FLOAT:
    //     valueStr = std::to_string(pair.second.asFloat());
    //     break;
    //   case cocos2d::Value::Type::STRING:
    //     valueStr = pair.second.asString();
    //     break;
    //   default:
    //     valueStr = "未知类型";
    //     break;
    //   }
    //   CCLOG("键: %s, 值类型: %d, 值: %s", pair.first.c_str(),
    //         static_cast<int>(pair.second.getType()), valueStr.c_str());
    // }

    if (!object.empty()) {
      // 获取数据，添加存在性检查
      int gid = object.find("gid") != object.end() ? object["gid"].asInt() : 0;
      float x = object.find("x") != object.end() ? object["x"].asFloat() : 0.0f;
      float y = object.find("y") != object.end() ? object["y"].asFloat() : 0.0f;
      float width = object.find("width") != object.end()
                        ? object["width"].asFloat()
                        : 0.0f;
      float height = object.find("height") != object.end()
                         ? object["height"].asFloat()
                         : 0.0f;

      // 打印调试信息
      CCLOG("传递给创建精灵的数据:");
      CCLOG("位置: (%.2f, %.2f), 尺寸: %.2f x %.2f", x, y, width, height);

      // 创建精灵
      auto sprite = cocos2d::Sprite::create(spritePath);
      if (sprite) {
        sprite->setAnchorPoint(cocos2d::Vec2(0, 0));

        // 获取精灵的原始尺寸
        cocos2d::Size spriteSize = sprite->getContentSize();

        // 计算缩放比例以匹配对象的尺寸
        // float scaleX = (width > 0) ? width / spriteSize.width : 1.0f;
        // float scaleY = (height > 0) ? height / spriteSize.height : 1.0f;
        // sprite->setScale(scaleX, scaleY);
        // CCLOG("精灵缩放比例: scaleX = %.2f, scaleY = %.2f", scaleX, scaleY);

        // 设置精灵的位置
        sprite->setPosition(cocos2d::Vec2(x, y + height));
        CCLOG("已设置精灵位置，具体为：(%.2f, %.2f)", x, y);

        // // 设置 Z 顺序，确保在基础层之上
        // sprite->setLocalZOrder(1);

        // // 创建基本对象
        // GameObject *gameObject = new BasicObject(sprite);

        // // 应用装饰器
        // if (layerProps.physical) {
        //   GameObject *physicsObject = new PhysicsAdder(gameObject);
        //   gameObject = physicsObject;
        // }

        // if (layerProps.boundary) {
        //   GameObject *boundaryObject = new BoundaryAdder(gameObject);
        //   gameObject = boundaryObject;
        // }

        // 调用渲染
        // gameObject->render();

        // 添加精灵到地图
        tmxMap->addChild(sprite, 100);

        // 清理内存
        // delete gameObject;

        // 添加调试边框（可选）
        auto debugRectSprite = cocos2d::DrawNode::create();
        cocos2d::Vec2 rectVertices[4] = {
            cocos2d::Vec2(0, 0), cocos2d::Vec2(width, 0),
            cocos2d::Vec2(width, height), cocos2d::Vec2(0, height)};
        debugRectSprite->drawRect(rectVertices[0], rectVertices[2],
                                  cocos2d::Color4F::GREEN);

        // 添加位置标记（可选）
        auto marker = cocos2d::DrawNode::create();
        marker->drawDot(cocos2d::Vec2(x, y + height), 5,
                        cocos2d::Color4F::BLUE);
        tmxMap->addChild(marker, 200); // 确保标记在房屋精灵之上
        sprite->addChild(debugRectSprite);
        CCLOG("已添加调试边框到精灵");
      } else {
        CCLOG("无法加载精灵，路径：%s", spritePath.c_str());
      }
    }
  }
}
