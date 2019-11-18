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
    CameraComponent(Rendering::SceneObject* sceneObject);

    void Update(float deltaTime) override;
};

} // namespace Components

