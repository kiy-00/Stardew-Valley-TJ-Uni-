#pragma once
// FarmlandManager.h
#pragma once
#include "cocos2d.h"
#include "FarmlandTile.h"
#include <unordered_map>
#include<vector>

USING_NS_CC;

class FarmlandManager {
public:
    static FarmlandManager* getInstance() {
        if (!instance) {
            instance = new FarmlandManager();
        }
        return instance;
    }

    // FarmlandManager.h
    bool init(cocos2d::Scene* scene, const std::vector<Vec2>& farmablePositions, int zOrder);

    // 处理工具使用事件
    void handleToolAction(const std::string& toolType, const Vec2& position, int direction);

    // 获取指定位置的耕地状态
    FarmlandTile* getFarmlandAt(const Vec2& position);

    std::vector<FarmlandTile*> getallFarmlands()const
    {

        std::vector<FarmlandTile*>lands;
        for (auto& land : farmlands)
        {
            lands.push_back(land.second);
        }

        return lands;
    }

    // 检查位置是否可以耕种
    bool isFarmable(const Vec2& position) const;

    // 更新所有耕地状态（可用于定时更新）
    void update(float dt);

private:
    FarmlandManager() {}
    static FarmlandManager* instance;

    cocos2d::Layer* farmlandLayer;
    std::unordered_map<std::string, FarmlandTile*> farmlands; // 使用string化的坐标作为key
    std::vector<Vec2> farmablePositions;

    // 将Vec2转换为string key
    std::string positionToString(const Vec2& pos) const {
        return std::to_string(int(pos.x)) + "," + std::to_string(int(pos.y));
    }
};