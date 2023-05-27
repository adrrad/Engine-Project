#pragma once
#include "components/BaseComponent.hpp"
#include "rendering/RenderingStructs.hpp"

namespace Gameplay
{
class CameraComponent;
class WaveManagerComponent;
class RigidBodyComponent;

class InspectorCameraComponent : public BaseComponent
{

private:
    float m_rotationSpeed;
    float m_movementSpeed;
    glm::vec3 eulerOffset = glm::vec3(0);
    bool forward = false;
    bool backward = false;
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool m_mouseLocked = true;
    CameraComponent* camera;
    RigidBodyComponent* rigidbodycomp = nullptr;
public:

    InspectorCameraComponent();

    void Start() override;

    void Update(float deltaTime) override;

    void SetCamera(CameraComponent* camera);


};

} // namespace Gameplay

