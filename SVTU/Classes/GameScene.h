#ifndef __GAME_SCENE_H__

#define __GAME_SCENE_H__



#include "FarmConfigManager.h"

#include "cocos2d.h"



class GameScene : public cocos2d::Scene {

public:

  // 创建场景的静态方法

  static cocos2d::Scene *createScene();



  // 初始化方法

  virtual bool init();



  const cocos2d::Node *getMapContainer() const { return mapContainer; }



  // 初始化瓦片地图的方法

  bool initTileMap();



  // 创建房屋精灵的辅助方法

  void createHouseSprite(cocos2d::TMXTiledMap *map, int gid, float x, float y,

                         float width, float height);



  // 加载房屋精灵的方法

  void loadHouseSprites();



  // 检查指定位置是否可通过

  bool isPositionWalkable(const cocos2d::Vec2 &position);



  // 初始化玩家的方法

  bool initPlayer();



  // 设置相机的方法

  void setupCamera();



  // 随机移动玩家的方法

  void movePlayerRandomly(float dt);



  // 更新方法

  virtual void update(float dt) override;



  // 自定义绘制方法

  virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform,

                    uint32_t flags) override;



  void addCollisionToLayer(cocos2d::TMXLayer *layer, bool boundary,

                           bool physical);



  void applyLayerProperties(const FarmTypeConfig &config);



  void

  applyLayers(const std::unordered_map<std::string, LayerProperties> &layers,

              const std::string &layerType);

  void addPhysicalToLayer(cocos2d::TMXLayer *layer);



  void addBoundaryToLayer(cocos2d::TMXLayer *layer);



  void loadObjectLayers();



  // 实现创建函数

  CREATE_FUNC(GameScene);



private:

  cocos2d::TMXTiledMap *tmxMap;

  cocos2d::Sprite *player;

  cocos2d::Node *mapContainer;



  cocos2d::TMXLayer *baseLayer;



  float _mapWidth;  // 地图实际宽度

  float _mapHeight; // 地图实际高度



  // 新增键盘处理方法

  void setupKeyboard();

  void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode,

                    cocos2d::Event *event);

};



#endif // __GAME_SCENE_H__

