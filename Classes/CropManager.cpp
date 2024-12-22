#include "CropManager.h"

USING_NS_CC;

CropManager* CropManager::s_instance = nullptr;

CropManager* CropManager::getInstance()
{
    if (!s_instance)
    {
        s_instance = new (std::nothrow) CropManager();
    }
    return s_instance;
}

CropManager::CropManager()
{
}

CropManager::~CropManager()
{
    // 这里可根据需要做收尾，比如移除所有crop
    // 但一般在游戏退出时由Ref机制清理即可
}

bool CropManager::init(cocos2d::Scene* scene, int zOrder)
{
    this->_cropLayer = cocos2d::Layer::create();

    if (!_cropLayer)
        return false;

    scene->addChild(_cropLayer, zOrder);

    return true;

}

Crop* CropManager::plantCrop(const std::string& cropType, FarmlandTile* tile)
{
    if (!tile) {
        CCLOG("CropManager::plantCrop failed: tile is nullptr");
        return nullptr;
    }

    // 如果这块地上已经有作物，就不再重复种植（也可选择先 remove 再种植）
    auto it = _cropMap.find(tile);
    if (it != _cropMap.end() && it->second != nullptr) {
        CCLOG("CropManager::plantCrop failed: tile already has a Crop.");
        return nullptr;
    }

    // 创建Crop对象。注意，你的Crop::create签名是 (type, tile)
    auto newCrop = Crop::create(cropType, tile);
    if (!newCrop) {
        CCLOG("CropManager::plantCrop failed: Crop::create returned null for type=%s", cropType.c_str());
        return nullptr;
    }

    // 设置作物在与地块对应的世界坐标位置
    // 因为FarmlandTile是Node，所以 tile->getPosition() 即世界坐标
    auto pos = tile->getPosition();
    pos.x += 16.0f;
    pos.y += 16.0f;
    newCrop->setPosition(pos);

    // 将作物（Node）添加到图层，便于渲染
    if (_cropLayer) {
        _cropLayer->addChild(newCrop);
    }

    // 记录到地图
    _cropMap[tile] = newCrop;

    CCLOG("CropManager::plantCrop success: Planted %s at tile(%p).", cropType.c_str(), tile);

    return newCrop;
}

Crop* CropManager::getCropOnTile(FarmlandTile* tile) const
{
    auto it = _cropMap.find(tile);
    if (it != _cropMap.end()) {
        return it->second;
    }
    return nullptr;
}

bool CropManager::removeCrop(FarmlandTile* tile, bool alsoCleanup)
{
    auto it = _cropMap.find(tile);
    if (it == _cropMap.end() || it->second == nullptr) {
        CCLOG("CropManager::removeCrop: No crop found on tile(%p).", tile);
        return false;
    }

    // 获取对应的作物
    Crop* crop = it->second;

    // 如果需要清理事件监听和从场景移除
    if (alsoCleanup) {
        crop->removeFromParent();
    }

    // 在map中移除
    _cropMap.erase(it);

    CCLOG("CropManager::removeCrop: Crop removed from tile(%p).", tile);
    return true;
}
