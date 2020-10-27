#pragma once
#include "utilities/json/JSON.hpp"

#include <memory>

namespace Engine
{

class Serialisable
{

public:
    virtual std::shared_ptr<Utilities::JSON::JSONValue> Serialise() = 0;

    virtual void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) = 0;
}; 

} // namespace Engine
