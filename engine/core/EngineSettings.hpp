#pragma once
#include "EngineTypedefs.hpp"

#include <string>

namespace Core
{

struct EngineSettings
{
    friend class EngineCore;

    struct 
    {
        std::string ProjectName;
        std::string RootDirectory;
    } Project;
    struct
    {
        WindowHandle MainWindow;
        WindowSize Width;
        WindowSize Height;
        bool Maximized;
    } Window;  
    struct
    {
        u64 TotalMemory;
        u64 GameplayMemory;
        u64 SceneMemory;
        u64 ComponentMemory;
        u64 RendererMemory;
        u64 PhysicsMemory;
        u64 UtilitiesMemory;
        u64 AssetsMemory;
    } Memory;
    struct Rendering
    {
        u64 MaxTextures;
    };
    

    static const EngineSettings& Get()
    {
        if(s_instance == nullptr) s_instance = new EngineSettings();
        return *s_instance;
    }

private:

    EngineSettings() 
    {

    };

    static EngineSettings& CreateEngineSettingsInstance()
    {
        if(s_instance == nullptr)
        {
            s_instance = new EngineSettings();
        } 
        return *s_instance;
    }

    static EngineSettings* s_instance;
};

} // namespace Core
