#include "FishDataFactory.h"


////////////////////////////////////////////////////////////
// 辅助函数，用于将“任何天气”拆分为晴天和雨天两条记录。
// @param fishTable 鱼类信息表。
// @param season 季节。
// @param name 鱼的名称。
// @param sprite 鱼的精灵图像名称。
// @param gold 鱼的金币价值。
// @param hp 鱼的生命值。
// @param energy 鱼的能量值。
////////////////////////////////////////////////////////////
static void addFishForBothWeathers(std::vector<FishInfo>& fishTable, const std::string& season, const std::string& name,
	const std::string& sprite, int gold, int hp, int energy) {
	fishTable.push_back({ season,"晴天",name,sprite,gold,hp,energy });  // 添加晴天记录
	fishTable.push_back({ season,"雨天",name,sprite,gold,hp,energy });  // 添加雨天记录
}

////////////////////////////////////////////////////////////
// 为每条鱼添加所有季节、所有天气的记录
// @param fishTable 鱼类信息表。
// @param name 鱼的名称。
// @param sprite 鱼的精灵图像名称。
// @param gold 鱼的金币价值。
// @param hp 鱼的生命值。
// @param energy 鱼的能量值。
////////////////////////////////////////////////////////////
static void addFishAllSeasonsAndWeathers(std::vector<FishInfo>& fishTable, const std::string& name,
	const std::string& sprite, int gold, int hp, int energy) {
	// 所有季节
	std::vector<std::string> seasons = { "春季","夏季","秋季","冬季" };
	// 所有天气
	std::vector<std::string> weathers = { "晴天","雨天" };

	// 遍历所有季节和天气，生成相应的鱼记录
	for (auto& s : seasons) {
		for (auto& w : weathers) {
			fishTable.push_back({ s,w,name,sprite,gold,hp,energy });
		}
	}
}

////////////////////////////////////////////////////////////
// 创建并返回所有鱼类数据
// FishInfo : 季节、天气、鱼名、金币、生命值和能量值
////////////////////////////////////////////////////////////
std::vector<FishInfo> FishDataFactory::createFishData() {
	std::vector<FishInfo> fishTable;

	/* -------------- 春季 ------------- */
	// 任何天气
	addFishForBothWeathers(fishTable, "春季", "Anchovy", "fish/Anchovy.png", 50, 20, 30);
	addFishForBothWeathers(fishTable, "春季", "Herring", "fish/Herring.png", 80, 20, 30);

	// 雨天
	fishTable.push_back({ "春季","雨天","Catfish","fish/Catfish.png",300,20,30 });
	fishTable.push_back({ "春季","雨天","Shad","fish/Shad.png",100,20,30 });
	fishTable.push_back({ "春季","雨天","Eel","fish/Eel.png",200,20,30 });

	// 晴天
	fishTable.push_back({ "春季","晴天","Sunfish","fish/Sunfish.png",150,20,30 });

	/* -------------- 夏季 ------------- */
	// 任何天气
	addFishForBothWeathers(fishTable, "夏季", "Tuna", "fish/Tuna.png", 300, 20, 30);
	addFishForBothWeathers(fishTable, "夏季", "Red Mullet", "fish/Red Mullet.png", 150, 20, 30);
	addFishForBothWeathers(fishTable, "夏季", "Pike", "fish/Pike.png", 250, 20, 30);
	addFishForBothWeathers(fishTable, "夏季", "Sturgeon", "fish/Sturgeon.png", 1000, 20, 30);

	// 雨天
	fishTable.push_back({ "夏季","雨天","Catfish","fish/Catfish.png",300,20,30 });
	fishTable.push_back({ "夏季","雨天","Shad","fish/Shad.png",100,20,30 });
	fishTable.push_back({ "夏季","雨天","Red Snapper","fish/Red Snapper.png",200,20,30 });

	// 晴天
	fishTable.push_back({ "夏季","晴天","Pufferfish","fish/Pufferfish.png",100,20,30 });
	fishTable.push_back({ "夏季","晴天","Sunfish","fish/Sunfish.png",150,20,30 });

	/* -------------- 秋季 ------------- */
	// 任何天气
	addFishForBothWeathers(fishTable, "秋季", "Anchovy", "fish/Anchovy.png", 50, 20, 30);
	addFishForBothWeathers(fishTable, "秋季", "Smallmouth Bass", "fish/Smallmouth Bass.png", 100, 20, 30);
	addFishForBothWeathers(fishTable, "秋季", "Salmon", "fish/Salmon.png", 200, 20, 30);
	addFishForBothWeathers(fishTable, "秋季", "Midnight Carp", "fish/Midnight Carp.png", 200, 20, 30);

	// 雨天
	fishTable.push_back({ "秋季","雨天","Catfish","fish/Catfish.png",300,20,30 });
	fishTable.push_back({ "秋季","雨天","Shad","fish/Shad.png",100,20,30 });
	fishTable.push_back({ "秋季","雨天","Red Snapper","fish/Red Snapper.png",200,20,30 });
	fishTable.push_back({ "秋季","雨天","Eel","fish/Eel.png",200,20,30 });
	fishTable.push_back({ "秋季","雨天","Walleye","fish/Walleye.png",200,20,30 });

	/* -------------- 冬季 ------------- */
	// 任何天气
	addFishForBothWeathers(fishTable, "冬季", "Tuna", "fish/Tuna.png", 300, 20, 30);
	addFishForBothWeathers(fishTable, "冬季", "Pike", "fish/Pike.png", 250, 20, 30);
	addFishForBothWeathers(fishTable, "冬季", "Herring", "fish/Herring.png", 80, 20, 30);
	addFishForBothWeathers(fishTable, "冬季", "Red Snapper", "fish/Red Snapper.png", 200, 20, 30);
	addFishForBothWeathers(fishTable, "冬季", "Squid", "fish/Squid.png", 150, 20, 30);
	addFishForBothWeathers(fishTable, "冬季", "Sturgeon", "fish/Sturgeon.png", 1000, 20, 30);

	// 雨天
	fishTable.push_back({ "冬季","雨天","Red Mullet","fish/Red Mullet.png",150,20,30 });

	/* -------------- 四季 ------------- */
	// 所有天气
	addFishAllSeasonsAndWeathers(fishTable, "Trash", "fish/Trash.png", 0, 0, 0);			// 垃圾
	addFishAllSeasonsAndWeathers(fishTable, "Paper", "fish/Paper.png", 0, 0, 0);			// 报纸
	addFishAllSeasonsAndWeathers(fishTable, "JoJa Cola", "fish/JoJa Cola.png", 0, 10, 15);  // JoJa可乐

	return fishTable;  // 返回所有生成的鱼类数据
}
