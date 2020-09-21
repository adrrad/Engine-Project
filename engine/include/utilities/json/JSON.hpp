#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Engine::Utilities::JSON
{
enum class JSONValueType { NUMBER, STRING, BOOLEAN, ARRAY, OBJECT, JSON_NULL };
struct JSONValue;



struct JSONObject
{
    std::unordered_map<std::string, JSONValue*> Members;

    inline JSONObject(std::unordered_map<std::string, JSONValue*> members)
    {
        Members = members;
    }

    inline JSONValue* operator[](std::string key)
    {
        if(Members.contains(key)) return Members[key];
        throw "No member '" + key + "' found!"; 
    }
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

    inline JSONValue* operator[](std::string key)
    {
        if(Type == JSONValueType::OBJECT) return (*Object)[key];
        throw "Cannot index a JSON value that is not an object!";
    }

    std::string String;
    std::vector<JSONValue*> Array;

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
};

struct JSONKeyValuePair
{
    std::string Name;
    JSONValue* Value;
};

JSONObject* ParseJSON(std::string& jsonString);

} // namespace Engine::Utilities::JSON
