#pragma once
#include "EngineTypedefs.hpp"

#include <imgui.h>

#include <unordered_map>
#include <stdint.h>

namespace Engine::Editor
{
    
struct GUIProperties
{
    static uint32_t WindowWidth;
    static uint32_t WindowHeight;
    static std::unordered_map<std::string, ImFont*> Fonts;
    static void LoadFont(std::string fontName, AssetID fontAssetID);
};

} // namespace Engine::Editor
