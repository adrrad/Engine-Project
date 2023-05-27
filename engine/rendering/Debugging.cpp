#include "rendering/Debugging.hpp"

LastAPICallInfo* LastAPICallInfo::s_instance = nullptr;

std::string LastAPICallInfo::ToString()
{
    return "Last API Call:\n    File: " + File +
    "\n    Line: " + std::to_string(Line) + 
    "\n    Function: " + Function + 
    "\n    Extra Info: " + ExtraInfo;
}

LastAPICallInfo& LastAPICallInfo::GetInstance()
{
    if(s_instance == nullptr) s_instance = new LastAPICallInfo();
    return *s_instance;
}