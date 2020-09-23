#pragma once

#include "utilities/StringUtilities.hpp"
#include <vector>

namespace Engine::Utilities::JSON
{
enum class JSONValueType { NUMBER, STRING, BOOLEAN, ARRAY, OBJECT, JSON_NULL };
struct JSONValue;

struct JSONKeyValuePair
{
    std::string Key;
    JSONValue* Value;

    inline JSONKeyValuePair() {};

    inline JSONKeyValuePair(std::string key, JSONValue* value)
    {
        Key = key;
        Value = value;
    }
};


struct JSONObject
{
    std::vector<JSONKeyValuePair> Members;

    inline JSONObject(std::vector<JSONKeyValuePair> members)
    {
        Members = members;
    }

    inline JSONValue* operator[](std::string key)
    {
        for(auto& kv : Members) if(kv.Key == key) return kv.Value;
        throw "No member '" + key + "' found!"; 
    }

    std::string ToString(int indent = 0);
};

struct JSONValue
{
    const JSONValueType Type;
    union
    {
        double Number;
        bool Boolean;
        JSONObject* Object;
    };
    std::string String;
    std::vector<JSONValue*> Array;

    inline JSONValue* operator[](std::string key)
    {
        if(Type == JSONValueType::OBJECT) return (*Object)[key];
        throw "Cannot index a JSON value that is not an object!";
    }

    inline JSONValue() : Type(JSONValueType::JSON_NULL)
    {

    }

    inline JSONValue(double val) : Type(JSONValueType::NUMBER)
    {
        Number = val;
    }

    inline JSONValue(std::string val) : Type(JSONValueType::STRING)
    {
        String = val;
    }

    inline JSONValue(bool val) : Type(JSONValueType::BOOLEAN)
    {
        Boolean = val;
    }

    inline JSONValue(std::vector<JSONValue*> val) : Type(JSONValueType::ARRAY)
    {
        Array = val;
    }

    inline JSONValue(JSONObject* val) : Type(JSONValueType::OBJECT)
    {
        Object = val;
    }
    
    ~JSONValue()
    {
        if(Type == JSONValueType::OBJECT) delete Object;
    }

    std::string ToString(int indent = 0);
};


JSONValue* ParseJSON(std::string& jsonString);

} // namespace Engine::Utilities::JSON
