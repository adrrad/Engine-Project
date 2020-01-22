#pragma once

#include "components/BaseComponent.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Cubemap.hpp"
#include "renderer/Material.hpp"

namespace Components
{

class SkyboxComponent : public BaseComponent
{
friend class Rendering::Renderer;
private:
    Rendering::Cubemap* _cubemap = nullptr;
    Rendering::Mesh* _cube = nullptr;
    Rendering::Material* _material = nullptr;
public:
    
    SkyboxComponent();

    void Update(float deltaTime) override;

    void SetTextures(Rendering::Texture* right, Rendering::Texture* left, Rendering::Texture* top, Rendering::Texture* bot, Rendering::Texture* back, Rendering::Texture* front);
};

} // namespace Components

