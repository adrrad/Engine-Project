#pragma once

#include "utilities/StringUtilities.hpp"
#include <vector>
#include <memory>

namespace Engine::Utilities::JSON
{
    
enum class JSONValueType { NUMBER, FLOAT, STRING, BOOLEAN, ARRAY, OBJECT, JSON_NULL };
struct JSONValue;

struct JSONKeyValuePair
{
    std::string Key;
    std::shared_ptr<JSONValue> Value;

    inline JSONKeyValuePair() {};
    
    inline JSONKeyValuePair(std::string key, std::shared_ptr<JSONValue> value)
    {
        Key = key;
        Value = value;
    }
};


struct JSONValue
{
    const JSONValueType Type;
    union
    {
        double Number;
        float Float;
        bool Boolean;
    };
    std::string String;
    std::vector<std::shared_ptr<JSONValue>> Array;
    std::vector<JSONKeyValuePair> Members;

    inline std::shared_ptr<JSONValue> GetMember(std::string key)
    {
        if(Type != JSONValueType::OBJECT) throw "Not a JSON object!";
        for(auto& kv : Members) if(kv.Key == key) return kv.Value;
        throw "No member '" + key + "' found!"; 
    }

    inline std::shared_ptr<JSONValue> operator[](std::string key)
    {
        return GetMember(key); 
    }

    inline JSONValue() : Type(JSONValueType::JSON_NULL)
    {

    }

    inline JSONValue(double val) : Type(JSONValueType::FLOAT)
    {
        Float = val;
    }

    inline JSONValue(float val) : Type(JSONValueType::FLOAT)
    {
        Float = val;
    }

    inline JSONValue(std::string val) : Type(JSONValueType::STRING)
    {
        String = val;
    }

    inline JSONValue(bool val) : Type(JSONValueType::BOOLEAN)
    {
        Boolean = val;
    }

    inline JSONValue(std::vector<std::shared_ptr<JSONValue>> val) : Type(JSONValueType::ARRAY)
    {
        Array = val;
    }

    inline JSONValue(std::vector<JSONKeyValuePair> members) : Type(JSONValueType::OBJECT)
    {
        Members = members;
    }
    
    ~JSONValue()
    {
        
    }

    std::string ToString(int indent = 0);


    inline static std::shared_ptr<JSONValue> AsNull()
    {
        return std::make_shared<JSONValue>();
    }

    inline static std::shared_ptr<JSONValue> AsNumber(double val)
    {
        return std::make_shared<JSONValue>(val);
    }

    inline static std::shared_ptr<JSONValue> AsFloat(float val)
    {
        return std::make_shared<JSONValue>(val);
    }

    inline static std::shared_ptr<JSONValue> AsString(std::string val)
    {
        return std::make_shared<JSONValue>(val);
    }

    inline static std::shared_ptr<JSONValue> AsBoolean(bool val)
    {
        return std::make_shared<JSONValue>(val);
    }

    inline static std::shared_ptr<JSONValue> AsArray(std::vector<std::shared_ptr<JSONValue>> val)
    {
        return std::make_shared<JSONValue>(val);
    }

    inline static std::shared_ptr<JSONValue> AsObject(std::vector<JSONKeyValuePair> val)
    {
        return std::make_shared<JSONValue>(val);
    }

};


std::shared_ptr<JSONValue> ParseJSON(std::string& jsonString);

} // namespace Engine::Utilities::JSON
