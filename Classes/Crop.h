#pragma once
#ifndef __CROP_H__
#define __CROP_H__

#include "cocos2d.h"
#include "Position.h"
#include "Enums.h"
#include "CropGrowthState.h"
#include "ICropQualityState.h"

USING_NS_CC;
class IThreatState;
class Crop : public Sprite {
public:
	Crop() {};
	static Crop* create(const std::string& cropName);
	virtual bool init() override;
	virtual ~Crop();

	// 基本属性访问器
	virtual CropType getCropType()const = 0;//获取作物种类
	virtual std::vector<SeasonType> getSupportSeason() const = 0;//获取作物生长的季节
	virtual int getGrowthDays() const = 0;
	GrowthStage getGrowthStage() const { return m_growthState->getStage(); }
	CropQuality getQuality() const { return m_qualityState->getQuality(); }
	int getDaysToMature() const { return m_daysToMature; }//获取成长所需总天数
	bool isReadyToHarvest() const { return m_growthState->getStage() == GrowthStage::MATURE; }

	// 生长相关
	virtual void grow(float dt);
	void setGrowthRate(float rate) { m_growthRate = rate; }//设置生长速率
	float getGrowthRate() const { return m_growthRate; }
	void updateGrowthFromLand(bool isWatered, bool isFertilized, float fertility);
	void advanceGrowthStage();
	void finalizeQuality();
	virtual void updateAppearance();

	// 状态相关
	void setGrowthState(ICropGrowthState* state);
	void setQualityState(ICropQualityState* newState);
	void setQualityPoints(float newPoints) { m_qualityPoints = newPoints; }
	float getQualityPoints() { return m_qualityPoints; }

	//灾害相关
	void updateThreatTimer(float dt);
	void checkNewThreats();//随机产生灾害威胁
	void setThreatState(IThreatState* state);
	ThreatType getThreatType();

	// 健康状态
	float getHealth() const { return m_health; }
	bool isDead() const { return m_health <= 0; }
	void died();
	void onDamage(float damage);

	// 收获相关
	virtual int getBaseValue() const = 0;
	virtual int getHarvestValue() const = 0;

protected:
	// 基本属性
	float m_growthProgress;     // 当前阶段的生长进度 (0-1)，每次到1就进入下一阶段
	int m_daysToMature;         // 成熟所需天数
	IThreatState* m_threatState = nullptr;//记录当前威胁状态
	float m_threatTimer = 0;//正常状态持续时间
	const float THREAT_CHECK_INTERVAL = 5.0f;  //随机产生新威胁的时间间隔
	std::vector<SeasonType> m_growableSeasons;


	// 状态相关
	ICropGrowthState* m_growthState;
	ICropQualityState* m_qualityState;

	// 生长相关
	float m_growthRate;         // 生长速度修正
	float m_qualityPoints;      // 品质积累点数
	float m_health;             // 植物健康度 (0-100)

	// 贴图相关
	std::map<GrowthStage, std::string> m_stageTextures;
	
	virtual void initializeGrowthStages() = 0;

private:
	void updateQuality();
};

#endif // __CROP_H__