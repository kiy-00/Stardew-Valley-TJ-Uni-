#include "GameMap.h"
#include "MapBuilder.h"
#include "MapConstants.h"
#include "MapLoader.h"
#include "cocos2d.h"
class MapManager {
private:
  // 单例实现
  static MapManager *instance;
  MapManager();
  virtual ~MapManager();

  // 核心组件
  std::unique_ptr<MapLoader> mapLoader;   // 负责加载tmx地图
  std::unique_ptr<MapBuilder> mapBuilder; // 负责构建地图实例
  std::unique_ptr<GameMap> currentMap;    // 当前活跃的地图
  cocos2d::Follow *cameraFollow;          // 相机跟随动作

public:
  static MapManager *getInstance();
  static void destroyInstance();

  // 初始化
  bool init();

  // 地图加载流程控制
  bool loadMap(const std::string &tmxPath, MapConstants::MapType type);
  void unloadCurrentMap();

  // 地图访问
  GameMap *getCurrentMap() const { return currentMap.get(); }
  cocos2d::TMXTiledMap *getCurrentTMXMap() const;
  //  MapConstants::MapType getCurrentMapType() const;

  // 相机控制 (通过cocos2d的Follow Action实现)
  void setupCamera(cocos2d::Node *target); // 设置相机跟随目标(通常是玩家)
  void stopCamera();                       // 停止相机跟随

  // 视野范围判定
  bool isInViewport(const cocos2d::Vec2 &position) const;
  cocos2d::Rect getViewport() const;

  // 地图查询接口(代理到GameMap)
  // bool isWalkable(const cocos2d::Vec2 &position) const;
  // bool hasPhysical(const cocos2d::Vec2 &position) const;
  // bool isBoundary(const cocos2d::Vec2 &position) const;
  // bool isTransparent(const cocos2d::Vec2 &position) const;
};