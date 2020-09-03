#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Material.hpp"
#include "renderer/Texture.hpp"

namespace Rendering
{
class Renderer;    
}

namespace Components
{

class MeshComponent : public BaseComponent
{
friend class Rendering::Renderer;
friend class Rendering::Renderpass;
private:
    Rendering::Mesh* _mesh = nullptr;
    Rendering::Material* _material = nullptr;
    Rendering::Texture* _texture = nullptr;

    void DebugGUI();

public:
    glm::vec4 MeshColour = glm::vec4(0.0f);

    MeshComponent();

    void Update(float deltaTime) override;

    void DrawGUI();

    /**
     * @brief Set the Mesh object. Does not delete any previous mesh assigned.
     * 
     * @param mesh Pointer to the mesh object.
     */
    void SetMesh(Rendering::Mesh *mesh);

    /**
     * @brief Set the Material object. Does not delete any previous material assigned.
     * 
     * @param material Pointer to the material object.
     */
    void SetMaterial(Rendering::Material *material);

    void SetTexture(Rendering::Texture *texture);

    Engine::Geometry::Volume* GetBoundingVolume();
};

} // namespace Components

