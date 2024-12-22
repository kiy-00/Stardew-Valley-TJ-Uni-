#ifndef __CROP_ITEM_H__
#define __CROP_ITEM_H__

#include "Item.h"

class CropItem : public Item {
public:
    CropItem(const std::string& name, const std::string& description, const std::string& imagePath)
        : Item(name, description, "crop", imagePath) {
    }

};

#endif // __CROP_ITEM_H__