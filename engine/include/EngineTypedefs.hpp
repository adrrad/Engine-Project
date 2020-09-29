#pragma once

#include <stdint.h>
#include <cstring>
namespace Engine
{

struct GUID
{
    wchar_t value[64];

    inline GUID(wchar_t guid[64])
    {
        memcpy(value, guid, sizeof(wchar_t)*64);
    }
};


typedef uint32_t Index;
typedef uint32_t Offset;
typedef uint32_t WindowSize;
typedef uint32_t WindowHandle;
typedef uint64_t GameObjectID;
typedef uint64_t ComponentID;
typedef uint64_t ElementCount;
typedef uint64_t Capacity;
typedef uint64_t SizeBytes;

typedef GUID AssetID;

} // namespace Engine
