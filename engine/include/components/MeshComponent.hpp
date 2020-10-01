#pragma once

#include "components/BaseComponent.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Material.hpp"
#include "rendering/Texture.hpp"

#include "assets/resources/MeshAsset.hpp"
#include "assets/AssetManager.hpp"

#include <glm/glm.hpp>

namespace Engine::Rendering
{
class Renderer;    
}

namespace Engine::Components
{

class MeshComponent : public Component<MeshComponent>
{
friend class Rendering::Renderer;
friend class Rendering::Renderpass;
private:
    Rendering::Mesh* m_mesh = nullptr;
    Rendering::Material* m_material = nullptr;
    glm::vec3 _meshOffset;

    void DrawBB();
public:
    bool DrawBoundingBox = false;

    MeshComponent();

    void Start() override;

    void Update(float deltaTime) override;

    void DrawInspectorGUI() override;


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

    Engine::Geometry::Volume* GetBoundingVolume();

    void SetMeshOffset(glm::vec3 offset);

    glm::mat4 GetModelMatrix();

};

} // namespace Engine::Components

