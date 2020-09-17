#pragma once
#include "utilities/serialisation/Serialisation.hpp"

#define str(v) std::to_string(v)

namespace Engine::Utilities::Serialisation
{

std::unordered_map<std::string, std::vector<std::function<std::string(void*)>>> Serialiser::Serialisers;
// std::unordered_map<std::string, std::vector<std::function<std::string(void*)>>> Serialiser::Deserialisers;
std::set<std::string> Serialiser::SerializedProps;

} // namespace Utilities
