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
    SERIALISABLE(InspectorCameraComponent, float, _rotationSpeed);
    SERIALISABLE(InspectorCameraComponent, float, _movementSpeed);
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

    inline std::string GetSerialised(int indent) override
    {
        return Utilities::Serialisation::SerializeObject(this, indent);
    }

};

} // namespace Engine::Components

