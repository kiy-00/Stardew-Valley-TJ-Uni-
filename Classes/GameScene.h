// GameScene.h
#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Farmland.h"
#include "ToolManager.h"
#include "SeasonSystem.h"
#include <vector>

class GameScene : public cocos2d::Scene {
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(GameScene);
	~GameScene();

private:
	std::vector<Farmland*> m_farmlands;  // 农田数组

	ITool* m_currentTool;                // 当前手持的工具
	CropType m_currentCropType;			//当前手持的种子
	bool m_isHarvestMode;				//是否处于收获状态

	WeatherSystem* weatherSystem;//管理时间的系统
	SeasonSystem* seasonSystem;//管理季节的系统

	TMXTiledMap* tmxMap;

	void initFarmlands();    // 初始化农田网格
	void initToolButtons();  // 初始化工具按钮
	void initCropButtons();	//初始化种子按钮
	bool initTileMap();

	// 触摸事件处理
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);

	// 工具选择处理
	void onToolButtonClicked(Ref* sender, const std::string& toolType);
	void onCropButtonClicked(Ref* sender, CropType cropType);

	// 获取点击位置的农田
	Farmland* getFarmlandAtPosition(const cocos2d::Vec2& position);

	//定时任务
	void update(float dt)override;
};

#endif#pragma once
