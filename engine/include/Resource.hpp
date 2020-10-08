#pragma once

#include "EngineTypedefs.hpp"

namespace Engine
{

class Resource
{
private:
    const GUID m_resourceID;

protected:

public:

    Resource(GUID resourceID) : m_resourceID(resourceID) {}

    __forceinline GUID GetResourceID() { return m_resourceID; }

};

} // namespace Engine
