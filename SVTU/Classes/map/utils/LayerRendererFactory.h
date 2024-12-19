﻿// filepath: map/utils/LayerRendererFactory.h

#pragma once

#include <string>

#include <unordered_map>



class LayerRendererFactory {

public:

  static std::string getSpritePath(const std::string &layerName);

};