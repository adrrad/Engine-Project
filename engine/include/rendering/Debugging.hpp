#pragma once

#include <stdint.h>
#include <string>

#define UPDATE_CALLINFO() \
{ \
    auto& call_info = LastAPICallInfo::GetInstance(); \
    call_info.File = __FILE__; \
    call_info.Line = __LINE__; \
    call_info.Function = __func__; \
    call_info.ExtraInfo = ""; \
}

#define UPDATE_CALLINFO2(info) \
{ \
    auto& call_info = LastAPICallInfo::GetInstance(); \
    call_info.File = __FILE__; \
    call_info.Line = __LINE__; \
    call_info.Function = __func__; \
    call_info.ExtraInfo = info; \
}

class LastAPICallInfo
{
private: 
    LastAPICallInfo() = default;
    static LastAPICallInfo* m_instance;
public:
    uint32_t Line;
    std::string File;
    std::string Function;
    std::string ExtraInfo;
    std::string ToString();
    static LastAPICallInfo& GetInstance();
};

