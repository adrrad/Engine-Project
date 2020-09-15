#include "rendering/Debugging.hpp"

LastAPICallInfo* LastAPICallInfo::_instance;

std::string LastAPICallInfo::ToString()
{
    return "Last API Call:\n    File: " + File +
    "\n    Line: " + std::to_string(Line) + 
    "\n    Function: " + Function + 
    "\n    Extra Info: " + ExtraInfo;
}

LastAPICallInfo& LastAPICallInfo::GetInstance()
{
    if(_instance == nullptr) _instance = new LastAPICallInfo();
    return *_instance;
}