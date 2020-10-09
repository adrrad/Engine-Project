#pragma once

#include "EngineTypedefs.hpp"

namespace Engine
{

class Resource
{
private:
    const AssetID m_resourceID;

protected:

public:

    Resource(AssetID resourceID) : m_resourceID(resourceID) {}

    __forceinline AssetID GetResourceID() { return m_resourceID; }

};

} // namespace Engine
