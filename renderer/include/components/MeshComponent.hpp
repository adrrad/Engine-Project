#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Material.hpp"

namespace Rendering
{
class Renderer;    
}

namespace Components
{

class MeshComponent : public BaseComponent
{
friend class Rendering::Renderer;
private:
    Rendering::Mesh *_mesh = nullptr;
    Rendering::Material *_material = nullptr;
public:
    glm::vec4 MeshColour = glm::vec4(0.0f);

    MeshComponent();

    void Update(float deltaTime) override;

    void DrawGUI();

    void SetMesh(Rendering::Mesh *mesh);

    void SetMaterial(Rendering::Material *material);
};

} // namespace Components

