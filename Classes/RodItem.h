#ifndef __ROD_ITEM_H__
#define __ROD_ITEM_H__

#include "Item.h"

class RodItem : public Item {
public:
    RodItem(const std::string& name, const std::string& description, const std::string& imagePath)
        : Item(name, description, "rod", imagePath) {}

};

#endif // __ROD_ITEM_H__
