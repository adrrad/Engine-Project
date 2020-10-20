#include "editor/gui/GUIProperties.hpp"

#include "assets/AssetManager.hpp"
#include "assets/resources/FontAsset.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace Engine::Editor
{

uint32_t GUIProperties::WindowWidth;
uint32_t GUIProperties::WindowHeight;
std::unordered_map<std::string, ImFont*> GUIProperties::Fonts;


void GUIProperties::LoadFont(std::string fontName, AssetID fontAssetID)
{
    Assets::FontAsset* fontAsset = Assets::AssetManager::GetInstance()->GetAsset<Assets::FontAsset>(fontAssetID);
    std::string& fontData = fontAsset->GetFontData();
    ImFontAtlas* fontAtlas = ImGui::GetIO().Fonts;
    ImFont* font = fontAtlas->AddFontFromMemoryTTF(&fontData[0], 10, 10);
    Fonts.insert({fontName, font});
}

} // namespace Engine::Editor
