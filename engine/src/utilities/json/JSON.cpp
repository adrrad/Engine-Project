#include "utilities/json/JSON.hpp"
#include "utilities/json/JSONParser.hpp"

namespace Engine::Utilities::JSON
{



JSONObject* ParseJSON(std::string& jsonString)
{
    return JSONParser(jsonString).ParseToObject();
}

} // namespace Engine::Utilities::JSON
