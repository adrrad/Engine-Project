#include "rendering/Debugging.hpp"

LastAPICallInfo* LastAPICallInfo::m_instance;

std::string LastAPICallInfo::ToString()
{
    return "Last API Call:\n    File: " + File +
    "\n    Line: " + std::to_string(Line) + 
    "\n    Function: " + Function + 
    "\n    Extra Info: " + ExtraInfo;
}

LastAPICallInfo& LastAPICallInfo::GetInstance()
{
    if(m_instance == nullptr) m_instance = new LastAPICallInfo();
    return *m_instance;
}