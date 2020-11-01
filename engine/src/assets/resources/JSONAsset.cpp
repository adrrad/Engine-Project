#pragma once
#include "assets/resources/JSONAsset.hpp"
namespace Engine::Assets
{

JSONAsset::JSONAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void JSONAsset::Load()
{
    if(IsLoaded()) return;
    auto content = ResourceFile->ReadAll();
    std::string jsonstr = std::string(content.Data(), content.Length);
    m_json = Utilities::JSON::ParseJSON(jsonstr);
}

void JSONAsset::Free()
{
    if(!IsLoaded()) return;
    m_json = nullptr;
}

bool JSONAsset::IsLoaded()
{
    return m_json != nullptr;
}

std::shared_ptr<Utilities::JSON::JSONValue> JSONAsset::GetJSONObject()
{
    return m_json;
}

} // namespace Engine::Assets
