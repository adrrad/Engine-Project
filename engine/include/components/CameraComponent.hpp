#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/RenderingStructs.hpp"
#include "geometry/Frustum.hpp"

#include <glm/gtx/intersect.hpp>

namespace Components
{

class CameraComponent : public BaseComponent
{
private:
    Rendering::Camera _camera;
    Engine::Geometry::Frustum _viewFrustum;
public:

    float FieldOfView = 45.0f;
    float AspectRatio = 1.0f;
    float NearPlane = 0.1f;
    float FarPlane = 1000.0f;
    glm::vec4 BackgroundColour = glm::vec4(0.0f);

    CameraComponent();

    void Update(float deltaTime) override;

    void DrawInspectorGUI() override;

    void DrawGUI();

    void SetMain();

    glm::mat4 GetViewMatrix();

    glm::mat4 GetProjectionMatrix();

    Rendering::Ray ScreenPointToRay(glm::vec2 point);

    glm::vec3 ColPlaneAt(Rendering::Ray r, float height);

    Engine::Geometry::Frustum& GetViewFrustum();
};

} // namespace Components

