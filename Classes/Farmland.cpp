#include "Farmland.h"
#include "LandStates.h" 
#include "CropFactory.h"
#include "ISeasonState.h"
#include <vector>
USING_NS_CC;

/**
* @brief 创建农田对象的静态工厂方法
* @param pos 农田在网格中的位置
* @return 成功返回农田对象指针,失败返回nullptr
*/
Farmland* Farmland::create(const Position& pos) {
	Farmland* farmland = new (std::nothrow) Farmland();
	if (farmland && farmland->init(pos)) {
		farmland->autorelease();
		return farmland;
	}
	CC_SAFE_DELETE(farmland);
	return nullptr;
}

/**
* @brief 初始化农田对象
* @param pos 农田在网格中的位置
* @return 初始化成功返回true,失败返回false
* @details 设置初始状态、纹理和位置等
*/
bool Farmland::init(const Position& pos) {

	if (!Sprite::initWithFile("farmland/normal.png")) {
		return false;
	}

	m_gridPosition = pos;
	m_currentState = new NormalState();
	m_currentCrop = nullptr;

	// 初始化状态
	m_waterTimer=0.0f;
	m_fertilizeTimer=0.0f;
	m_fertility = 0.0f;

	setPosition(pos.x * 32, pos.y * 32);

	return true;
}

// 处理工具使用
bool Farmland::handleTool(ITool* tool) {
	if (!tool) 
		return false;
	return tool->useTool(this);
}


/**
* @brief 设置土地状态
* @param newState 新的状态对象指针
* @details 删除旧状态,设置新状态,并更新外观
*/
void Farmland::setState(ILandState* newState) {
	if (m_currentState) {
		delete m_currentState;
	}
	m_currentState = newState;
	updateAppearance();
}

/**
* @brief 开垦土地
* @details 调用当前状态的till方法处理开垦操作
*/
void Farmland::till() {
	if (m_currentState) {
		m_currentState->till(this);
		return;
	}
}

/**
* @brief 浇水
* @details 调用当前状态的water方法处理浇水操作
*/
void Farmland::water() {
	if (m_currentState) {
		m_currentState->water(this);
		//如果浇水的时候有作物，且作物没死，且正在经历干旱
		if (m_currentCrop && !m_currentCrop->isDead() && m_currentCrop->getThreatType() == ThreatType::DROUGHT)
			m_currentCrop->setThreatState(nullptr);//则消除当前的灾害
		return;
	}
}


void Farmland::fertilize() {
	if (m_currentState) {
		m_currentState->fertilize(this);
		return;
	}
}
/**
* @brief 种植作物
* @param crop 要种植的作物对象指针
* @details 调用当前状态的plant方法处理种植操作
*/
void Farmland::plant(CropType cropType) {
	if (m_currentState) {
		m_currentState->plant(this, cropType);
		return;
	}
}

bool Farmland::harvest() {
	if (!m_currentCrop || !m_currentCrop->isReadyToHarvest()) {
		CCLOG("Nothing to harvest!");
		return false;
	}

	// 获取作物价值
	int value = m_currentCrop->getHarvestValue();
	// 移除作物
	removeCrop();

	// TODO: 添加收获动画或效果
	CCLOG("Harvested crop! Value: %d", value);
	return true;
}

/**
* @brief 更新函数,继承自Sprite
* @param dt 时间增量
* @details 调用当前状态的update方法进行状态更新
*/
void Farmland::update(float dt) {
	// 更新土地状态
	m_currentState->update(this, dt);

	// 更新作物
	if (m_currentCrop != nullptr) {
		m_currentCrop->grow(dt);  // 调用作物的生长方法
		m_currentCrop->updateThreatTimer(dt);//更新作物的灾害累计时间
	}
}

/**
* @brief 更新农田外观
* @details 根据当前土地状态(是否浇水、是否开垦)设置对应的纹理
*/
void Farmland::updateAppearance() {
	setTexture(m_currentState->getTexturePath());
}


void Farmland::setCrop(Crop* newCrop) {
	m_currentCrop = newCrop;
}

void Farmland::removeCrop() {
	if (m_currentCrop) {
		m_currentCrop->removeFromParent();  // 从场景节点树中移除
		delete m_currentCrop;
	}
	m_currentCrop = nullptr;
}

void Farmland::onWeatherChanged(const WeatherType weatherType) {
	m_currentState->weatherEffect(this,weatherType);
}

void Farmland::setCropGrowthRate(float growthRate) {
	m_currentCrop->setGrowthRate(1.0f);
}

void Farmland::onSeasonChanged(SeasonSystem* seasonSystem) {
	if(m_currentCrop)
		seasonSystem->getSeasonState()->canCropGrow(m_currentCrop);
}
