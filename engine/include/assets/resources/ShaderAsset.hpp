#pragma once
#include "assets/Asset.hpp"

#include "EngineData.hpp"

namespace Engine::Assets
{

class ShaderAsset : public Asset
{
private:
    AssetID m_fragmentShaderID = {'\0'};    
    AssetID m_vertexShaderID = {'\0'};

    ShaderData* m_shaderData = nullptr;

public:
    ShaderAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

};

    
} // namespace Engine::Assets
