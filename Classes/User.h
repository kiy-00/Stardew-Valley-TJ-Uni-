#ifndef __USER_H__
#define __USER_H__

#include "cocos2d.h"
#include "Inventory.h"
#include "Item.h"


class User : public cocos2d::Sprite {
public:
	// ʹ��ָ�����Դ���Userʵ��
	static User* create(const std::string& name, const std::string& gender, int health, int energy, int money);

	// ��ʼ���û����ݲ����ض���֡
	virtual bool init(const std::string& name, const std::string& gender, int health, int energy, int money);

	// ��ɫ�ƶ����������ݷ�����½�ɫ״̬
	void moveUp();   // �����ƶ�
	void moveDown(); // �����ƶ�
	void moveLeft(); // �����ƶ�
	void moveRight();// �����ƶ�
	void stopMoving() { m_isMoving = false; }

	// ���ڸ��¶������붨ʱ������
	void updateAnimation(float dt);

	// �������Ե�getter��setter�����ڷ��ʺ��޸Ľ�ɫ�����ơ��Ա𡢽�Ǯ����������������Ϣ
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

	// ��ȡ���������Ա�����Ͳ�ѯ��Ʒ
	Inventory* getInventory() const { return inventory; }

	// ����ѡ����Ʒ��������������status������ʾ
	bool reduceSelectedItemQuantity(int quantity, bool status);

	// ��ȡ�����õ�ǰѡ�еı�����λ
	std::pair<int, int> getSelectedSlot() const { return selectedSlot; }
	void setSelectedSlot(int first, int second) { selectedSlot = std::make_pair(first, second); }

	// ��ȡ�����ý�ɫ�������ƶ�״̬
	int getDirection() const { return m_direction; }
	void setDirection(int direction) { m_direction = direction; }
	bool isMoving() const { return m_isMoving; }
	void setMoving(bool moving) { m_isMoving = moving; }

	// ������ع��ܣ��л�������ʾ�������λ�����±�����ʾ
	void toggleInventory();
	void onSlotClicked(int row, int col);
	void updateInventoryDisplay();
	void updateHeldItemSprite();
	bool getIsInventoryOpen() const { return isInventoryOpen; }
	void setIsInventoryOpen(bool open) { isInventoryOpen = open; }

	// �������ܣ�������Ʒ����ѡ����Ʒ
	void createInventoryBar();
	void selectItemFromInventory(int index);

	// ��ȡ��ǰѡ����Ʒ������ʾ����
	Item* getSelectedItem();
	cocos2d::Sprite* getHeldItemSprite() const;

	// ����ϵͳ���л���λͼ����ʾ
	void toggleSlotImage();
	bool getIsSlotImageOpen() const { return isSlotImageOpen; }
	void setIsSlotImageOpen(bool open) { isSlotImageOpen = open; }

	// ���߶�����أ�ִ�й��߶�������Ӷ����ߣ�
	void performToolAction();

	// ������͸���ȿ���
	void updateVisibility(bool isPenetrable) {
		this->setOpacity(isPenetrable ? 128 : 255);
	}

	// ����������ϵͳ��ʼ��
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

private:
	int m_updateCounter; // �������¼����������ڿ��ƶ���֡��

	// ��������
	std::string m_name;
	std::string m_gender;
	int m_health;
	int m_energy;
	int m_money;

	cocos2d::Sprite* heldItemSprite; // ��ʾ���г�����Ʒ�ľ���
	Inventory* inventory;            // ��ɫ�ı���
	cocos2d::Sprite* slotSprite = nullptr; // �����λͼ��ľ���

	int m_direction;  // ��ǰ��ɫ���� 0��1��2��3��
	bool m_isMoving;  // �Ƿ����ƶ�״̬

	int m_frameIndex; // ����֡����
	cocos2d::Vector<cocos2d::SpriteFrame*> m_animationFrames[4]; // �洢���������ĸ�����Ķ���֡
	bool isInventoryOpen; // �����Ƿ��
	const float slotSize = 30.0f;   // ÿ����λ�Ĵ�С
	cocos2d::Layer* inventoryLayer = nullptr; // ��ʾ������ͼ��
	std::pair<int, int> selectedSlot = { -1, -1 }; // ��ǰѡ�еĲ�λ����

	bool isSlotImageOpen = false; // �����λͼ���Ƿ���ʾ

	bool m_isPerformingToolAction = false; // �Ƿ����ڲ��Ź���ʹ�ö���

	cocos2d::Sprite* toolActionSprite; // ������ʾ���߶��������ľ���

	// ���ؽ�ɫ���߶���֡
	void loadAnimationFrames();

	// ���ݹ������ƺͷ����ȡ���߶���֡
	cocos2d::Vector<cocos2d::SpriteFrame*> getToolAnimationFrames(const std::string& toolName, int direction);
};

#endif // __USER_H__