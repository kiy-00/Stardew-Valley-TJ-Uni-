#ifndef __TOOL_ITEM_H__
#define __TOOL_ITEM_H__

#include "Item.h"

class ToolItem : public Item {
public:
    ToolItem(const std::string& name, const std::string& description, const std::string& imagePath)
        : Item(name, description, "tool", imagePath) {}

};

#endif // __TOOL_ITEM_H__