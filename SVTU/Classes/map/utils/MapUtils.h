﻿#ifndef __MAP_UTILS_H__

#define __MAP_UTILS_H__



#include "cocos2d.h"

#include "json/document.h"

#include "json/stringbuffer.h"

#include "json/writer.h"



USING_NS_CC;



class MapUtils {

public:

  // 将 ValueMap 转换为 JSON 字符串

  static std::string valueMapToJsonString(const ValueMap &valueMap);



private:

  // 辅助函数：将 Value 转换为 rapidjson::Value

  static void valueToJson(const Value &value, rapidjson::Value &jsonValue,

                          rapidjson::Document::AllocatorType &allocator);

};



#endif // __MAP_UTILS_H__