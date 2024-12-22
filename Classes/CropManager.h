#ifndef __CROP_MANAGER_H__
#define __CROP_MANAGER_H__

#include "cocos2d.h"
#include "Crop.h"
#include "FarmlandTile.h"

/**
 * CropManager 负责：
 * 1. 记录（FarmlandTile -> Crop）的映射。
 * 2. 提供播种、移除作物等接口。
 * 3. 让作物的可视化节点（Crop本身）添加到指定图层中。
 *
 * 注意：本示例不负责“定时更新”所有作物，因为在你的 Crop.cpp 里，
 *       已经对每株 Crop 调用了 schedule() 来更新自己。若需要统一管理更新，
 *       可在此管理器中遍历所有作物并调用 update()。
 */
class CropManager
{
public:
    /**
     * 获取单例
     */
    static CropManager* getInstance();

    /**
     * 初始化管理器，可传入一个图层，用于将 Crop 的节点添加到该图层渲染。
     * 若你的项目需要将作物渲染到和农田相同的图层，也可以直接传入 farmlandLayer。
     */
    bool init(cocos2d::Scene* scene, int zOrder);

    /**
     * 在指定的地块上播种一个指定类型的作物
     * @param cropType  作物种类(对应你Crop里用到的type，比如"Berry"等)
     * @param tile      作物要种植的地块(已经翻地/可种植)
     * @return 若成功创建，返回指向Crop的指针；若失败(如该Tile已有作物)，返回nullptr
     */
    Crop* plantCrop(const std::string& cropType, FarmlandTile* tile);

    /**
     * 根据地块获取当前作物（若地块上无作物，则返回nullptr）
     */
    Crop* getCropOnTile(FarmlandTile* tile) const;

    /**
     * 移除地块上对应的作物（若有的话）
     * @param tile  对应的地块
     * @param alsoCleanup 是否在移除时调用Crop的 removeEventListeners() 并 removeFromParent()
     * @return true：成功移除；false：没有找到对应的作物
     */
    bool removeCrop(FarmlandTile* tile, bool alsoCleanup = true);

private:
    // 构造 & 析构
    CropManager();
    ~CropManager();

    // 禁用拷贝构造和赋值
    CropManager(const CropManager&) = delete;
    CropManager& operator=(const CropManager&) = delete;

private:
    // 存储“地块 -> 作物”的映射
    std::unordered_map<FarmlandTile*, Crop*> _cropMap;

    // 用来渲染作物的图层（可与FarmlandManager的 farmlandLayer相同，或单独新建）
    cocos2d::Layer* _cropLayer = nullptr;

    // 单例静态实例
    static CropManager* s_instance;
};

#endif // __CROP_MANAGER_H__
#pragma once
