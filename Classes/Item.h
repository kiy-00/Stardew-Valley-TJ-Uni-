#pragma once
#ifndef __ITEM_H__
#define __ITEM_H__

#include <string>
#include "cocos2d.h"

class Item {
public:
    Item(const std::string& name, const std::string& description, const std::string& itemType, const std::string& imagePath, int quantity = 1)
        : m_name(name), m_description(description), m_itemType(itemType), m_imagePath(imagePath), m_quantity(quantity) {}

	virtual ~Item() {} 

    virtual std::string getName() const { return m_name; }
    virtual std::string getDescription() const { return m_description; }
    virtual std::string getItemType() const { return m_itemType; }
    virtual std::string getImagePath() const { return m_imagePath; }
    int getQuantity() const { return m_quantity; }
    void setQuantity(int quantity) { m_quantity = quantity; }
   

  

    // 获取对应的精灵用于展示
    virtual cocos2d::Sprite* getSprite() const {
        return cocos2d::Sprite::create(m_imagePath);
    }

private:
    std::string m_name;        // 物品名称
    std::string m_description; // 物品描述
    std::string m_itemType;    // 物品类型（如 "tool", "seed" 等）
    std::string m_imagePath;   // 图片路径
    int m_quantity;
};

#endif // __ITEM_H__