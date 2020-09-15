#pragma once
#include "EngineTypedefs.hpp"

#include <string>

namespace Engine::Core
{

struct EngineSettings
{
    struct 
    {
        std::string ProjectName;
    } Project;
    struct
    {
        WindowHandle MainWindow;
        WindowSize Width;
        WindowSize Height;
        bool Maximized;
    } Window;  
};
} // namespace Engine::Core
