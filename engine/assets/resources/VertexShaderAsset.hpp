#pragma once
#include "assets/Asset.hpp"

namespace Assets
{

class VertexShaderAsset : public Asset
{
private:
    std::string m_source;
public:
    VertexShaderAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

    inline const std::string& GetSource() { return m_source; }

};

} // namespace Assets