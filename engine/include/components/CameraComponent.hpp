#pragma once
#include "components/BaseComponent.hpp"
#include "rendering/RenderingStructs.hpp"
#include "geometry/Frustum.hpp"

#include <glm/gtx/intersect.hpp>

namespace Engine::Components
{

class CameraComponent : public Component<CameraComponent>
{
private:
    static CameraComponent* MainCamera;
    Rendering::Camera _camera;
    Engine::Geometry::Frustum _viewFrustum;
public:

    SERIALISABLE(CameraComponent, float, FieldOfView);
    SERIALISABLE(CameraComponent, float, AspectRatio);
    SERIALISABLE(CameraComponent, float, NearPlane);
    SERIALISABLE(CameraComponent, float, FarPlane);
    glm::vec4 BackgroundColour = glm::vec4(0.0f);

    CameraComponent();

    void Start() override;

    void Update(float deltaTime) override;

    void DrawInspectorGUI() override;

    void DrawGUI();

    void SetMain();

    glm::mat4 GetViewMatrix();

    glm::mat4 GetProjectionMatrix();

    Rendering::Ray ScreenPointToRay(glm::vec2 point);

    glm::vec3 ColPlaneAt(Rendering::Ray r, float height);

    Engine::Geometry::Frustum& GetViewFrustum();

    static CameraComponent* GetMainCamera();

};




} // namespace Engine::Components

