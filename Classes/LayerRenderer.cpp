#include "LayerRenderer.h"

// 渲染普通图层
void LayerRenderer::renderLayer(cocos2d::TMXLayer* layer,
    const std::string& layerName,
    const std::string& spritePath,
    const LayerProperties& layerProps) {
    ////CCLOG("成功渲染普通图层: %s, 精灵路径: %s", layerName.c_str(),
    //    spritePath.c_str());
}

// 渲染对象层
void LayerRenderer::renderObjectLayer(cocos2d::TMXObjectGroup* objectGroup,
    cocos2d::TMXTiledMap* tmxMap,
    const std::string& layerName,
    const std::string& spritePath,
    const LayerProperties& layerProps) {
    if (!objectGroup) {
       // CCLOG("未找到名为 '%s' 的对象组。", layerName.c_str());
        return;
    }

    const auto& objects = objectGroup->getObjects();
    //CCLOG("找到 %s 对象数量: %zu", layerName.c_str(), objects.size());

    for (const auto& obj : objects) {
        cocos2d::ValueMap object = obj.asValueMap();

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
           /* CCLOG("传递给创建精灵的数据:");
            CCLOG("位置: (%.2f, %.2f), 尺寸: %.2f x %.2f", x, y, width, height);*/

            // 创建精灵
            auto sprite = cocos2d::Sprite::create(spritePath);
            if (sprite) {
                sprite->setAnchorPoint(cocos2d::Vec2(0, 0));

                // 获取精灵的原始尺寸
                cocos2d::Size spriteSize = sprite->getContentSize();

                // 设置精灵的位置
                sprite->setPosition(cocos2d::Vec2(x, y + height));
                //CCLOG("已设置精灵位置，具体为：(%.2f, %.2f)", x, y);

                // 添加物理体
                if (layerProps.physical) {
                    auto physicsBody = cocos2d::PhysicsBody::createBox(
                        cocos2d::Size(width, height),
                        cocos2d::PhysicsMaterial(0.1f, 0.5f, 0.5f)  // 密度、弹性、摩擦力
                    );

                    // 设置物理体属性
                    physicsBody->setDynamic(false);  // 静态物理体
                    physicsBody->setGravityEnable(false);  // 禁用重力
                    physicsBody->setCategoryBitmask(0xFFFFFFFF);    // 所有类别
                    physicsBody->setCollisionBitmask(0xFFFFFFFF);   // 与所有类别碰撞
                    physicsBody->setContactTestBitmask(0xFFFFFFFF); // 检测所有碰撞

                    // 将物理体附加到精灵
                    sprite->setPhysicsBody(physicsBody);

                    //CCLOG("已添加物理属性到精灵");
                }

                // 添加精灵到地图
                tmxMap->addChild(sprite, 100);

                // 添加调试边框
                auto debugRectSprite = cocos2d::DrawNode::create();
                cocos2d::Vec2 rectVertices[4] = {
                    cocos2d::Vec2(0, 0),
                    cocos2d::Vec2(width, 0),
                    cocos2d::Vec2(width, height),
                    cocos2d::Vec2(0, height)
                };
                debugRectSprite->drawRect(rectVertices[0], rectVertices[2],
                    cocos2d::Color4F::GREEN);

                // 添加位置标记
                auto marker = cocos2d::DrawNode::create();
                marker->drawDot(cocos2d::Vec2(x, y + height), 5,
                    cocos2d::Color4F::BLUE);
                tmxMap->addChild(marker, 200); // 确保标记在房屋精灵之上
                sprite->addChild(debugRectSprite);
                //CCLOG("已添加调试边框到精灵");
            }
            else {
                //CCLOG("无法加载精灵，路径：%s", spritePath.c_str());
            }
        }
    }
}