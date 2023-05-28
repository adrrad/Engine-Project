#include "utilities/json/JSON.hpp"
#include "utilities/json/JSONParser.hpp"

#include "EngineTypedefs.hpp"

namespace Utilities::JSON
{



std::shared_ptr<JSONValue> ParseJSON(std::string& jsonString)
{
    if(jsonString.length() == 0) return nullptr;
    return JSONParser(jsonString).ParseToObject();
}


std::string JSONValue::ToString(int indent)
{
    switch (Type)
    {
    case JSONValueType::NUMBER:
        return std::to_string(Number);
    case JSONValueType::FLOAT:
        return std::to_string(Float);
    case JSONValueType::STRING:
        return "\""+String+"\"";
    case JSONValueType::JSON_NULL:
        return "null";
    case JSONValueType::BOOLEAN:
        return Boolean ? "true" : "false";
    case JSONValueType::OBJECT:
    {
        std::string i = Utilities::Repeat("    ", indent);
        std::string i2 = Utilities::Repeat("    ", indent+1);
        std::string nl = std::string("\n");
        std::string qt = "\"";
        std::string object = nl + i + "{" + nl;
        for(u64 index = 0; index < Members.size(); index++)
        {
            auto& member = Members[index];
            object += i2 + qt + member.Key + qt + " : ";
            object += member.Value->ToString(indent+1);
            if(index < Members.size()-1) object += ", ";
            object += nl;
        }
        object += i + "}";
        return object;
    }
    case JSONValueType::ARRAY:
    {
        std::string array = " [ ";
        for(u64 index = 0; index < Array.size(); index++)
        {
            array += Array[index]->ToString(indent);
            if(index < Array.size()-1) array += ", ";
        }
        array += " ]";
        return array;
    }

    default:
        throw std::exception("Uknown JSON value type!");
    }
}

} // namespace Utilities::JSON
