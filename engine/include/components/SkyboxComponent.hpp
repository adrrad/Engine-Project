#pragma once

#include "components/BaseComponent.hpp"
#include "renderer/Skybox.hpp"

namespace Components
{

class SkyboxComponent : public BaseComponent
{
friend class Rendering::Renderer;
private:
    Rendering::Texture* _skyboxTexture = nullptr;
    Rendering::Mesh* _cube = nullptr;
    Rendering::Material* _material = nullptr;
    Rendering::Skybox _skybox;

    void SetSkybox();

public:
    
    SkyboxComponent();

    void Update(float deltaTime) override;

    void SetSkyboxTexture(Rendering::Texture* skyboxTexture);
};

} // namespace Components

