#include "MapUtils.h"

void MapUtils::valueToJson(const Value &value, rapidjson::Value &jsonValue,
                           rapidjson::Document::AllocatorType &allocator) {
  switch (value.getType()) {
  case Value::Type::BOOLEAN:
    jsonValue.SetBool(value.asBool());
    break;
  case Value::Type::INTEGER:
    jsonValue.SetInt(value.asInt());
    break;
  case Value::Type::FLOAT:
  case Value::Type::DOUBLE:
    jsonValue.SetDouble(value.asDouble());
    break;
  case Value::Type::STRING:
    jsonValue.SetString(value.asString().c_str(), allocator);
    break;
  case Value::Type::VECTOR: {
    jsonValue.SetArray();
    const auto &vector = value.asValueVector();
    for (const auto &item : vector) {
      rapidjson::Value jsonItem;
      valueToJson(item, jsonItem, allocator);
      jsonValue.PushBack(jsonItem, allocator);
    }
    break;
  }
  case Value::Type::MAP: {
    jsonValue.SetObject();
    const auto &map = value.asValueMap();
    for (const auto &pair : map) {
      rapidjson::Value jsonKey(pair.first.c_str(), allocator);
      rapidjson::Value jsonVal;
      valueToJson(pair.second, jsonVal, allocator);
      jsonValue.AddMember(jsonKey, jsonVal, allocator);
    }
    break;
  }
  case Value::Type::INT_KEY_MAP: {
    jsonValue.SetObject();
    const auto &map = value.asIntKeyMap();
    for (const auto &pair : map) {
      std::string keyStr = std::to_string(pair.first);
      rapidjson::Value jsonKey(keyStr.c_str(), allocator);
      rapidjson::Value jsonVal;
      valueToJson(pair.second, jsonVal, allocator);
      jsonValue.AddMember(jsonKey, jsonVal, allocator);
    }
    break;
  }
  default:
    jsonValue.SetNull();
    break;
  }
}

std::string MapUtils::valueMapToJsonString(const ValueMap &valueMap) {
  rapidjson::Document document;
  document.SetObject();
  rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

  for (const auto &pair : valueMap) {
    rapidjson::Value key(pair.first.c_str(), allocator);
    rapidjson::Value value;
    valueToJson(pair.second, value, allocator);
    document.AddMember(key, value, allocator);
  }

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);
  return buffer.GetString();
}