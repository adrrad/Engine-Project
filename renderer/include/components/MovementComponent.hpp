#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/RenderingStructs.hpp"

namespace Components
{

class MovementComponent : public BaseComponent
{
private:
    float _rotationSpeed = 50.0f;
    float _movementSpeed = 3.0f;

    bool forward = false;
    bool backward = false;
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool _mouseLocked = true;
public:

    MovementComponent();

    void Update(float deltaTime) override;

};

} // namespace Components

