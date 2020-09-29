#include "platform/GUID.hpp"

#include <guiddef.h>
#include <combaseapi.h>

#include <iostream>

typedef GUID winGUID;

namespace Engine::Platform
{

GUID GenerateGUID()
{
    winGUID guid;
    if(CoCreateGuid(&guid) == S_OK)
    {
        wchar_t guidstr[64] = {0};
        StringFromGUID2(guid, guidstr, 64);
        return { guidstr };
    }
    throw std::exception("Could not generate a GUID!");
}

} // namespace Engine::Platform
