#pragma once

#include <stdint.h>
#include <cstring>
#include <string>

namespace Engine
{

struct GUID
{
private:
    std::string value = "00000000-0000-0000-0000-000000000000";

public:
    inline GUID()
    {
        
    }

    inline GUID(wchar_t guid[64])
    {
        for(int i = 0; i < 36; i++) value[i] = char(guid[i+1]);
    }

    inline GUID(std::string guid)
    {
        if(guid.size() == 36) value = guid;
    }

    inline std::string ToString() const
    {
        return value;
    }

    inline bool operator==(const GUID& other)
    {
        return value == other.value;
    }

    inline bool operator!=(const GUID& other)
    {
        return value != other.value;
    }

    inline bool IsAssigned() { return value != std::string("00000000-0000-0000-0000-000000000000"); }

};


typedef uint32_t Index;
typedef uint32_t Offset;
typedef uint32_t Size;
typedef uint32_t WindowSize;
typedef uint32_t WindowHandle;
typedef uint64_t ComponentID;
typedef uint64_t ElementCount;
typedef uint64_t Capacity;
typedef uint64_t SizeBytes;

typedef GUID AssetID;
typedef GUID GameObjectID;

} // namespace Engine
