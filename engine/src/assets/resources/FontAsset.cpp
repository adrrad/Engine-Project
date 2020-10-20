#pragma once
#include "assets/resources/FontAsset.hpp"

namespace Engine::Assets
{

FontAsset::FontAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void FontAsset::Load()
{
    Array<char> content = ResourceFile->ReadAll();
    m_fontData = std::string(content.Data(), content.Length);
}

void FontAsset::Free()
{
    m_fontData = "";
}

bool FontAsset::IsLoaded()
{
    return m_fontData != "";
}

std::string& FontAsset::GetFontData()
{
    return m_fontData;
}

} // namespace Engine::Assets
