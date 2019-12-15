#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/RenderingStructs.hpp"

namespace Components
{
class CameraComponent;
class WaveManagerComponent;
class MovementComponent : public BaseComponent
{

private:
    float _rotationSpeed = 50.0f;
    float _movementSpeed = 10.0f;

    bool forward = false;
    bool backward = false;
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool _mouseLocked = true;
    CameraComponent* camera;
    WaveManagerComponent* waveManager;
public:

    MovementComponent();

    void Update(float deltaTime) override;

    void SetCamera(CameraComponent* camera);

    void SetWaveManager(WaveManagerComponent* waveManager);

};

} // namespace Components

