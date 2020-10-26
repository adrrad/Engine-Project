#pragma once
#include "components/BaseComponent.hpp"
#include "rendering/RenderingStructs.hpp"
#include "rendering/Cubemap.hpp"
#include "geometry/Frustum.hpp"
#include "assets/resources/CubemapAsset.hpp"

#include <glm/gtx/intersect.hpp>

namespace Engine::Rendering
{
    class Renderer;
    class Texture;
}

namespace Engine::Components
{

class CameraComponent : public BaseComponent
{
friend class Engine::Rendering::Renderer;

private:
    static CameraComponent* MainCamera;
    Rendering::Camera _camera;
    Engine::Geometry::Frustum _viewFrustum;
    Rendering::Cubemap* m_skyboxCubemap = nullptr;
public:

    float FieldOfView;
    float AspectRatio;
    float NearPlane;
    float FarPlane;
    glm::vec4 BackgroundColour = glm::vec4(0.0f);

    CameraComponent();

    void Start() override;

    void Update(float deltaTime) override;

    void DrawInspectorGUI() override;

    void DrawGUI();

    void SetMain();

    void SetSkybox(Assets::CubemapAsset* cubemap);

    glm::mat4 GetViewMatrix();

    glm::mat4 GetProjectionMatrix();

    Rendering::Ray ScreenPointToRay(glm::vec2 point);

    glm::vec3 ColPlaneAt(Rendering::Ray r, float height);

    Engine::Geometry::Frustum& GetViewFrustum();

    static CameraComponent* GetMainCamera();

};




} // namespace Engine::Components

