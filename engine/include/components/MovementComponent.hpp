#pragma once
#include "components/BaseComponent.hpp"
#include "rendering/RenderingStructs.hpp"

namespace Components
{
class CameraComponent;
class WaveManagerComponent;
class RigidBodyComponent;

class MovementComponent : public BaseComponent
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
    WaveManagerComponent* waveManager;
    RigidBodyComponent* rigidbodycomp = nullptr;
public:

    MovementComponent();

    void Start() override;

    void Update(float deltaTime) override;

    void SetCamera(CameraComponent* camera);

    void SetWaveManager(WaveManagerComponent* waveManager);

    void DrawInspectorGUI() override;

};

} // namespace Components

