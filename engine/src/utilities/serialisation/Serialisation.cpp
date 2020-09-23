#pragma once
#include "utilities/serialisation/Serialisation.hpp"

#define str(v) std::to_string(v)

namespace Engine::Utilities::Serialisation
{

bool Serialiser::initialised = false;
std::unordered_map<std::string, std::vector<std::function<JSON::JSONKeyValuePair(void*)>>>* Serialiser::Serialisers;
std::unordered_map<std::string, std::vector<std::function<void(void*, JSON::JSONValue&)>>>* Serialiser::Deserialisers;
std::set<std::string>* Serialiser::SerialisedProps;

} // namespace Engine::Utilities::Serialisation
