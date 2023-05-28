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

}

typedef Engine::GUID AssetID;
typedef Engine::GUID ComponentID;
typedef Engine::GUID GameObjectID;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
