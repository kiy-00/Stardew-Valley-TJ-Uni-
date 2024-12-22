#ifndef __USER_H__
#define __USER_H__

#include "cocos2d.h"
#include "Inventory.h"
#include "Item.h"
#include "FishItem.h"
#include "FishingLayer.h"
#include "FishDataFactory.h"
#include "FarmMapManager.h"

class FishingLayer;

class User : public cocos2d::Sprite {
public:
	// 使用指定属性创建User实例
	static User* create(const std::string& name, const std::string& gender, int health, int energy, int money);

	// 初始化用户数据并加载动画帧
	virtual bool init(const std::string& name, const std::string& gender, int health, int energy, int money);

	// 角色移动方法，根据方向更新角色状态
	void moveUp();   // 向上移动
	void moveDown(); // 向下移动
	void moveLeft(); // 向左移动
	void moveRight();// 向右移动
	void stopMoving() { m_isMoving = false; }

	// 定期更新动画，与定时器相结合
	void updateAnimation(float dt);

	// 基本属性的getter和setter，用于访问和修改角色的名称、性别、金钱、健康、能量等信息
	const std::string& getName() const { return m_name; }
	void setName(const std::string& name) { m_name = name; }
	const std::string& getGender() const { return m_gender; }
	void setGender(const std::string& gender) { m_gender = gender; }
	const int getMoney() const { return m_money; }
	void setMoney(int money) { m_money = money; }
	int getHealth() const { return m_health; }
	void setHealth(int health) { m_health = health; }
	int getEnergy() const { return m_energy; }
	void setEnergy(int energy) { m_energy = energy; }

	// 获取背包对象，以便管理和查询物品
	Inventory* getInventory() const { return inventory; }

	// 减少选中物品的数量，并根据status更新显示
	bool reduceSelectedItemQuantity(int quantity, bool status);

	// 获取和设置当前选中的背包槽位
	std::pair<int, int> getSelectedSlot() const { return selectedSlot; }
	void setSelectedSlot(int first, int second) { selectedSlot = std::make_pair(first, second); }

	// 获取和设置角色方向与移动状态
	int getDirection() const { return m_direction; }
	void setDirection(int direction) { m_direction = direction; }
	bool isMoving() const { return m_isMoving; }
	void setMoving(bool moving) { m_isMoving = moving; }

	// 背包相关功能：切换背包显示、点击槽位、更新背包显示
	void toggleInventory();
	void onSlotClicked(int row, int col);
	void updateInventoryDisplay();
	void updateHeldItemSprite();
	bool getIsInventoryOpen() const { return isInventoryOpen; }
	void setIsInventoryOpen(bool open) { isInventoryOpen = open; }

	// 横栏功能：创建物品栏和选择物品
	void createInventoryBar();
	void selectItemFromInventory(int index);

	// 获取当前选中物品和其显示精灵
	Item* getSelectedItem();
	cocos2d::Sprite* getHeldItemSprite() const;

	// 制造系统：切换槽位图像显示
	void toggleSlotImage();
	bool getIsSlotImageOpen() const { return isSlotImageOpen; }
	void setIsSlotImageOpen(bool open) { isSlotImageOpen = open; }

	/* 执行当前持有物品的对应动作 */
	void performAction(FarmMapManager* farmMapManager);
	void performToolAction();  // 执行基本工具动作（如挥动工具）
	void plantSeed();		   // 执行播种逻辑
	void startFishing();       // 执行钓鱼逻辑

	// 新增：透明度控制
	void updateVisibility(bool isPenetrable) {
		this->setOpacity(isPenetrable ? 128 : 255);
	}

	// 新增：物理系统初始化
	void initPhysics() {
		cocos2d::Size playerSize = this->getContentSize();
		auto physicsBody = cocos2d::PhysicsBody::createBox(
			playerSize,
			cocos2d::PhysicsMaterial(1.0f, 0.0f, 1.0f)
		);

		physicsBody->setDynamic(true);
		physicsBody->setRotationEnable(false);
		physicsBody->setGravityEnable(false);
		physicsBody->setCategoryBitmask(0xFFFFFFFF);
		physicsBody->setCollisionBitmask(0xFFFFFFFF);
		physicsBody->setContactTestBitmask(0xFFFFFFFF);
		physicsBody->setEnabled(true);
		physicsBody->setTag(1);

		this->setPhysicsBody(physicsBody);
	}

	/* --------新增（用于拾取地图中的物品）------- */
	cocos2d::EventListenerTouchOneByOne* createPickupListener(Item* item, cocos2d::Sprite* sprite);

	void pickupNearbyItems(const std::vector<cocos2d::Sprite*>& items, float pickupRadius);

private:
	// 基本属性
	std::string m_name;
	std::string m_gender;
	int m_health;
	int m_energy;
	int m_money;
	int m_updateCounter;

	cocos2d::Sprite* heldItemSprite;		// 显示手中持有物品的精灵
	Inventory* inventory;					// 角色的背包
	cocos2d::Sprite* slotSprite = nullptr;  // 制造槽位图像的精灵

	int m_direction;  // 当前角色方向 0下1上2左3右
	bool m_isMoving;  // 是否处于移动状态
	int m_frameIndex; // 动画帧索引

	cocos2d::Vector<cocos2d::SpriteFrame*> m_animationFrames[4]; // 存储上下左右四个方向的动画帧

	bool isInventoryOpen; // 背包是否打开
	const float slotSize = 30.0f;   // 每个槽位的大小
	cocos2d::Layer* inventoryLayer = nullptr; // 显示背包的图层
	std::pair<int, int> selectedSlot = { -1, -1 }; // 当前选中的槽位坐标

	bool isSlotImageOpen = false; // 制造槽位图像是否显示

	bool m_isPerformingToolAction = false; // 是否正在播放工具使用动画

	cocos2d::Sprite* toolActionSprite; // 用于显示工具动作动画的精灵

	// 加载角色行走动画帧
	void loadAnimationFrames();

	// 根据工具名称和方向获取工具动画帧
	cocos2d::Vector<cocos2d::SpriteFrame*> getToolAnimationFrames(const std::string& toolName, int direction);
};

#endif // __USER_H__