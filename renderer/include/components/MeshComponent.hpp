#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/Mesh.hpp"

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
    Rendering::Mesh *_mesh;
public:
    glm::vec4 MeshColour = glm::vec4(0.0f);

    MeshComponent();

    void Update(float deltaTime) override;

    void DrawGUI();

    void SetMesh(Rendering::Mesh *mesh);
};

} // namespace Components

