#pragma once
#include "components/BaseComponent.hpp"
#include "rendering/RenderingStructs.hpp"

namespace Engine::Components
{
class CameraComponent;
class WaveManagerComponent;
class RigidBodyComponent;

class InspectorCameraComponent : public BaseComponent
{

private:
    float _rotationSpeed = 50.0f;
    float _movementSpeed = 10.0f;
    glm::vec3 eulerOffset = glm::vec3(0);
    bool forward = false;
    bool backward = false;
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool _mouseLocked = true;
    CameraComponent* camera;
    RigidBodyComponent* rigidbodycomp = nullptr;
public:

    InspectorCameraComponent();

    void Start() override;

    void Update(float deltaTime) override;

    void SetCamera(CameraComponent* camera);

    void DrawInspectorGUI() override;

};

} // namespace Engine::Components

