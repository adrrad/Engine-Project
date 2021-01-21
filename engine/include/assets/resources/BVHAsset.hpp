#pragma once
#include "assets/Asset.hpp"

#include "utilities/bvh/BVHAnimation.hpp"

namespace Engine::Assets
{

class BVHAsset : public Asset
{
private:
    Utilities::BVH::BVHAnimation *m_animation = nullptr;
public:
    BVHAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

    inline const Utilities::BVH::BVHAnimation* GetBVHAnimation() { return m_animation; }

};

    
} // namespace Engine::Assets
