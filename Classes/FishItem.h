#ifndef __FISH_ITEM_H__
#define __FISH_ITEM_H__

#include "Item.h"

class FishItem : public Item {
public:
	FishItem(const std::string& name, const std::string& description, const std::string& imagePath)
		: Item(name, description, "fish", imagePath) {}

};

#endif // __FISH_ITEM_H__
