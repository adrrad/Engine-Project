#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/Texture.hpp"
#include "renderer/Material.hpp"
#include "renderer/Mesh.hpp"


#include <vector>

namespace Components
{

class TerrainComponent : public BaseComponent
{

private:
    std::vector<Rendering::Mesh*> _segments;
    Rendering::Texture* _heightmap;
    Rendering::Material* _material;

public:

    TerrainComponent() : BaseComponent("Terrain Component") {}
    
    void Initialize(Rendering::Texture* heightmap, float XZscale, float heightscale);

    void Update(float deltaTime) override;

};

}