#pragma once
#include "assets/Asset.hpp"

namespace Engine::Assets
{

class MeshAsset : public Asset
{
private:

    char* m_meshData;

public:

    MeshAsset(Platform::IO::File* resourceFile);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

};


} // namespace Engine::Assets
