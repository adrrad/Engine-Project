#pragma once

#include <stdint.h>
#include <cstring>
#include <string>

namespace Engine
{

struct GUID
{
    char value[37] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

    inline GUID()
    {
        
    }

    inline GUID(wchar_t guid[64])
    {
        for(int i = 0; i < 36; i++) value[i] = char(guid[i+1]);
        value[36] = '\0';
    }

    inline std::string ToString() const
    {
        return std::string(value);
    }

};


typedef uint32_t Index;
typedef uint32_t Offset;
typedef uint32_t Size;
typedef uint32_t WindowSize;
typedef uint32_t WindowHandle;
typedef uint64_t GameObjectID;
typedef uint64_t ComponentID;
typedef uint64_t ElementCount;
typedef uint64_t Capacity;
typedef uint64_t SizeBytes;

typedef GUID AssetID;

} // namespace Engine
