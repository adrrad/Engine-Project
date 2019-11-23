#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/RenderingStructs.hpp"

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
    float FarPlane = 100.0f;

    CameraComponent();

    void Update(float deltaTime) override;

    void SetMain();
};

} // namespace Components

