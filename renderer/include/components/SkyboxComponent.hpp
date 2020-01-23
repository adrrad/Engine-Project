#pragma once

#include "components/BaseComponent.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Material.hpp"

namespace Components
{

class SkyboxComponent : public BaseComponent
{
friend class Rendering::Renderer;
private:
    Rendering::Texture* _skybox = nullptr;
    Rendering::Mesh* _cube = nullptr;
    Rendering::Material* _material = nullptr;
public:
    
    SkyboxComponent();

    void Update(float deltaTime) override;

    void SetSkyboxTexture(Rendering::Texture* skybox);
};

} // namespace Components

