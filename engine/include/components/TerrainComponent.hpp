#pragma once
#include "components/BaseComponent.hpp"
#include "rendering/Texture.hpp"
#include "rendering/Material.hpp"
#include "rendering/Mesh.hpp"


#include <vector>

// namespace Engine::Components
// {

// class TerrainComponent : public BaseComponent
// {

// private:
//     std::vector<Rendering::Mesh*> _segments;
//     Rendering::Texture* _heightmap;
//     Rendering::Material* _material;

// public:

//     TerrainComponent() : BaseComponent("Terrain Component") {}

//     void Initialize(Rendering::Texture* heightmap, float XZscale, float heightscale);
    
//     void Start() override;

//     void Update(float deltaTime) override;

// };

// }