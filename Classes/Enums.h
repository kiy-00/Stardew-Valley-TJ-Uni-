#pragma once
#ifndef __ENUMS_H__
#define __ENUMS_H__
enum class CropType {
	NONE,
	CORN,
	POTATO,
	TOMATO,
	WHEAT,
	CARROT
};

//工具种类
enum class ToolType {
	HOE,//锄头
	WATERING_CAN,//水壶
	FERTILIZER,//肥料
	PESTICIDE,//杀虫剂
	SICKLE//镰刀
};

//季节
enum class SeasonType {
	SPRING,
	SUMMER,
	AUTUMN,
	WINTER
};

//土地状态
enum class LandState {
	NORMAL,//正常
	TILLED,//开垦
	WATERED,//浇水
	FERTILIZED,//施肥
	WATERED_AND_FERTILIZED//浇水且施肥
};

//作物状态
enum class GrowthStage {
	SEED,       // 种子
	SPROUT,     // 发芽
	GROWING,    // 生长中
	MATURE,      // 成熟
	DEAD
};

enum class CropQuality {
	NORMAL,     // 普通
	SILVER,     // 银质
	GOLD,        // 金质
};

enum class WeatherType {
	SUNNY,      // 晴天
	SNOWY,     // 阴天
	RAINY,      // 雨天
	STORM,       // 暴风雨
};
enum class ThreatType {
	DROUGHT,
	DISEASE,
	NONE
};
#endif // __ENUMS_H__