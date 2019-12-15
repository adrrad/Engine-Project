#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/RenderingStructs.hpp"


#include <glm/gtx/intersect.hpp>

namespace Components
{

class CameraComponent : public BaseComponent
{
private:
    Rendering::Camera _camera;
public:
    //fovy, T aspect, T zNear, T zFar

    float FieldOfView = 45;
    float AspectRatio = 1.0f;
    float NearPlane = 0.1f;
    float FarPlane = 1000.0f;
    glm::vec4 BackgroundColour = glm::vec4(0.0f);

    CameraComponent();

    void Update(float deltaTime) override;

    void DrawGUI();

    void SetMain();

    glm::mat4 GetViewMatrix();

    glm::mat4 GetProjectionMatrix();

    Rendering::Ray ScreenPointToRay(glm::vec2 point);

    glm::vec3 ColPlaneAt(Rendering::Ray r, float height);
};

} // namespace Components

