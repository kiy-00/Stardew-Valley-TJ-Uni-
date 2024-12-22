#ifndef __BREED_ITEM_H__
#define __BREED_ITEM_H__

#include "Item.h"

class BreedItem : public Item {
public:
	BreedItem(const std::string& name, const std::string& description, const std::string& imagePath)
		: Item(name, description, "breed", imagePath) {}

};

#endif // __BREED_ITEM_H__

