// GameScene.cpp
#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene() {
	return GameScene::create();
}
GameScene::~GameScene() {
	if (weatherSystem) {
		weatherSystem->release();  // 释放天气系统单例
		weatherSystem = nullptr;
	}
	if (seasonSystem) {
		weatherSystem->release();  // 释放季节系统单例
		weatherSystem = nullptr;
	}
}
bool GameScene::init() {
	if (!Scene::init()) {
		return false;
	}

	// 获取天气系统实例
	weatherSystem = WeatherSystem::getInstance();

	// 获取季节系统实例
	seasonSystem = SeasonSystem::getInstance();

	// 初始化农田网格
	initFarmlands();

	// 初始化工具按钮
	initToolButtons();

	//初始化农作物按钮
	initCropButtons();
	

	// 注册触摸事件
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	//设置定时任务
	this->scheduleUpdate();

	return true;
}
bool GameScene::initTileMap() {
	// 加载瓦片地图
	tmxMap = TMXTiledMap::create("island_.tmx");
	if (tmxMap) {
		this->addChild(tmxMap);
		tmxMap->setName("tmxMap");
		tmxMap->setPosition(cocos2d::Vec2(0, 0));

		CCLOG("地图大小: %.0f x %.0f", tmxMap->getMapSize().width,
			tmxMap->getMapSize().height);
		CCLOG("图块大小: %.0f x %.0f", tmxMap->getTileSize().width,
			tmxMap->getTileSize().height);

		return true;
	}
	else {
		CCLOG("加载地图失败，当前工作目录: %s",
			FileUtils::getInstance()->getDefaultResourceRootPath().c_str());
		return false;
	}
}
void GameScene::initFarmlands() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// 创建3x3的农田网格
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			Position pos(i, j);
			Farmland* farmland = Farmland::create(pos);

			weatherSystem->addObserver(farmland);//注册天气系统观察者
			seasonSystem->addObserver(farmland);//注册季节系统观察者

			// 设置农田位置，左边留出空间放工具按钮
			farmland->setPosition(origin.x + 200 + i * 64, origin.y + 100 + j * 64);
			this->addChild(farmland);
			m_farmlands.push_back(farmland);//存储到GameScene的农田数组中
		}
	}
}

void GameScene::initToolButtons() {
	// 创建工具按钮
	auto createToolButton = [this](const std::string& text, const std::string& toolType, float posY) {
		auto label = Label::createWithTTF(text, "fonts/arial.ttf", 20);
		auto button = MenuItemLabel::create(
			label,
			[this, toolType](Ref* sender) { onToolButtonClicked(sender, toolType); }
		);
		button->setPosition(Vec2(100, posY));
		return button;
	};

	auto menu = Menu::create(
		createToolButton("Hoe", "hoe", 300),
		createToolButton("Water", "wateringcan", 250),
		createToolButton("Fertilizer", "fertilizer", 200),
		nullptr
	);

	// 添加收获按钮
	auto harvestButton = MenuItemLabel::create(
		Label::createWithTTF("Harvest", "fonts/arial.ttf", 20),
		[this](Ref* sender) {
			if (m_currentTool) {
				delete m_currentTool;
			}
			m_currentTool = nullptr;  // 空手状态用于收获
			m_isHarvestMode = true;   // 进入收获模式
		}
	);

	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);
	harvestButton->setPosition(Vec2(100, 150));
	menu->addChild(harvestButton);

}
void GameScene::initCropButtons() {
	// 创建种植按钮
	auto createCropButton = [this](const std::string& text, CropType cropType, float posY) {
		auto label = Label::createWithTTF(text, "fonts/arial.ttf", 20);
		auto button = MenuItemLabel::create(
			label,
			[this, cropType](Ref* sender) { onCropButtonClicked(sender, cropType); }
		);
		button->setPosition(Vec2(100, posY));  // 放在工具按钮下面
		return button;
		};

	auto menu = Menu::create(
		createCropButton("Plant Corn", CropType::CORN, 100),
		//createCropButton("Plant Potato", CropType::POTATO, 100),
		//createCropButton("Plant Tomato", CropType::TOMATO, 50),
		nullptr
	);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);
}

// 修改触摸处理函数以支持种植
bool GameScene::onTouchBegan(Touch* touch, Event* event) {
	auto farmland = getFarmlandAtPosition(touch->getLocation());
	if (!farmland) return false;

	if (m_isHarvestMode) {
		// 收获模式
		if (farmland->harvest()) {
			// 收获成功
			m_isHarvestMode = false;  // 退出收获模式
		}
	}
	else if (m_currentTool) {
		// 使用工具
		farmland->handleTool(m_currentTool);
	}
	else {
		// 种植模式
		if (farmland->isTilled() && farmland->isWatered() && !farmland->hasCrop()) {
			farmland->plant(m_currentCropType);
		}
	}

	return true;
}

void GameScene::onToolButtonClicked(Ref* sender, const std::string& toolType) {
	if (m_currentTool) {
		delete m_currentTool;
	}
	m_currentCropType = CropType::NONE;
	m_currentTool = ToolManager::getInstance()->createTool(toolType);
	CCLOG("Selected tool: %s", toolType.c_str());
}

void GameScene::onCropButtonClicked(Ref* sender, CropType cropType) {
	m_currentCropType = cropType;
	CCLOG("Selected crop: %d", static_cast<int>(cropType));

	// 如果有作物被选中，自动切换到空手状态
	if (m_currentTool) {
		delete m_currentTool;
		m_currentTool = nullptr;
	}
}

Farmland* GameScene::getFarmlandAtPosition(const Vec2& position) {
	for (auto farmland : m_farmlands) {
		if (farmland->getBoundingBox().containsPoint(position)) {
			return farmland;
		}
	}
	return nullptr;
}

void GameScene::update(float dt) {
	// 加速时间流逝（可选）
	float timeScale = 150.0f;  // 1现实秒 = 1游戏分钟
	float gameTime = 0.01f * timeScale;

	// 更新所有农田和作物
	for (auto farmland : m_farmlands) {
		farmland->update(gameTime);
		weatherSystem->update(gameTime);
		seasonSystem->update(gameTime);
	}
}