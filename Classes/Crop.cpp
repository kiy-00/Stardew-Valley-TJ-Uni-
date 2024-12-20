#include "Crop.h"
#include "CropGrowthState.h"
#include "CropQualityStates.h"
#include "IThreatState.h"
#include "ThreatStates.h"

USING_NS_CC;

Crop* Crop::create(const std::string& cropName) {
	CCLOG("Cannot create instance of abstract class Crop");
	return nullptr;
}

Crop::~Crop() {
	if (m_growthState) delete m_growthState;
	if (m_qualityState) delete m_qualityState;
}

bool Crop::init() {
	if (!Sprite::init()) {
		return false;
	}

	// 初始化基本属性
	m_growthProgress = 0.0f;
	m_daysToMature = getGrowthDays();

	// 初始化生长相关属性
	m_growthRate = 1.0f;
	m_qualityPoints = 0.0f;
	m_health = 100.0f;

	// 初始化状态
	m_growthState = new SeedState();
	m_qualityState = new NormalQualityState();

	// 初始化生长阶段贴图
	initializeGrowthStages();
	updateAppearance();

	return true;
}

void Crop::grow(float dt) {
	if (m_growthState->getStage() == GrowthStage::MATURE || isDead()) {
		return;
	}

	// 计算实际生长量
	float baseGrowth = dt / (m_daysToMature * 24.0f);
	float actualGrowth = baseGrowth * m_growthRate;
	m_growthProgress += actualGrowth;

	// 检查是否需要进入下一阶段
	if (m_growthProgress >= 1.0f) {
		advanceGrowthStage();
	}

	// 更新品质
	updateQuality();
}

void Crop::updateGrowthFromLand(bool isWatered, bool isFertilized, float fertility) {
	// 更新生长
	float growthModifier = 1.0f;
	if (isWatered) growthModifier *= 1.5f;
	if (isFertilized) growthModifier *= 1.3f;
	m_growthRate = growthModifier;

	// 更新品质
	m_qualityState->updateQualityPoints(this, isWatered, isFertilized, fertility);
}

void Crop::advanceGrowthStage() {
	m_growthProgress = 0.0f;
	m_growthState->advance(this);
}

void Crop::setGrowthState(ICropGrowthState* state) {
	if (m_growthState) delete m_growthState;
	m_growthState = state;
	updateAppearance();
}

void Crop::setQualityState(ICropQualityState* state) {
	if (m_qualityState) delete m_qualityState;
	m_qualityState = state;
}

void Crop::died() {
	m_health = 0.0f;
	auto it = m_stageTextures.find(GrowthStage::DEAD);
	setTexture(it->second);
}

void Crop::onDamage(float damage) {
	m_health = std::max(0.0f, m_health - damage);
	if (m_health < 50.0f) {
		m_growthRate *= 0.8f;
	}
	if (isDead()) {
		died();
		CCLOG("%s has died!", m_name.c_str());
	}
}

int Crop::getHarvestValue() const {
	// 基础价格
	int basePrice = getBaseValue();

	// 品质加成倍率由品质状态决定
	float qualityMultiplier = m_qualityState->getPriceMultiplier();

	// 考虑作物健康度的影响
	float healthModifier = m_health / 100.0f;

	return static_cast<int>(basePrice * qualityMultiplier * healthModifier);
}

void Crop::updateQuality() {
	if (m_qualityPoints >= 100.0f) {
		setQualityState(new GoldQualityState());
	}
	else if (m_qualityPoints >= 50.0f) {
		setQualityState(new SilverQualityState());
	}
	else {
		setQualityState(new NormalQualityState());
	}
}

void Crop::finalizeQuality() {
	updateQuality();
	CCLOG("%s final quality: %d", m_name.c_str(), static_cast<int>(m_qualityState->getQuality()));
}

void Crop::updateAppearance() {
	auto it = m_stageTextures.find(m_growthState->getStage());
	if (it != m_stageTextures.end()) {
		setTexture(it->second);
	}
	// 如果有威胁，调整颜色
	if (m_threatState) {
		if (m_threatState->getType() == ThreatType::DROUGHT) {
			this->setColor(Color3B(255, 200, 150));  // 偏黄
		}
		else if (m_threatState->getType() == ThreatType::DISEASE) {
			this->setColor(Color3B(150, 255, 150));  // 偏绿
		}
	}
	else {
		this->setColor(Color3B::WHITE);  // 恢复正常颜色
	}
}

void Crop::updateThreatTimer(float dt) {
	// 更新现有威胁
	if (m_threatState) {//如果当前已有威胁，则调用该威胁的update对作物造成伤害
		m_threatState->update(this, dt);
	}
	else {//否则就累计正常状态的持续时间
		m_threatTimer += dt;
		if (m_threatTimer >= THREAT_CHECK_INTERVAL) {
			checkNewThreats();
			m_threatTimer = 0;
		}
	}

}

void Crop::checkNewThreats() {

	// 根据环境条件和随机概率生成威胁
	float random = CCRANDOM_0_1();
	if (random < 0.1f) {  // 10%概率
		setThreatState(new DroughtThreatState());
	}
	else if (random < 0.15f) {  // 5%概率
		setThreatState(new DiseaseThreatState());
	}
}

void Crop::setThreatState(IThreatState* state) {
	if (m_threatState) {
		delete m_threatState;
	}
	m_threatState = state;
	// 更新外观
	updateAppearance();
}

ThreatType Crop::getThreatType() {
	if (m_threatState)
		return m_threatState->getType();
	return ThreatType::NONE;
}
