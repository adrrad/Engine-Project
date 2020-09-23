#include "components/InspectorCameraComponent.hpp"
#include "platform/WindowManager.hpp"
#include "core/GameObject.hpp"
#include "rendering/Quaternion.hpp"
#include "components/RigidBodyComponent.hpp"
#include <imgui.h>

#include <iostream>
#include <algorithm>
#include <math.h>

using namespace glm;
using namespace std;


namespace Engine::Components
{
using namespace Engine::Rendering;
InspectorCameraComponent::InspectorCameraComponent() : Component(typeid(InspectorCameraComponent).name())
{
    auto winMan = Platform::WindowManager::GetInstance();
    winMan->RegisterMousePositionCallback([&](double dx, double dy)
    {
        if(_mouseLocked)
        {
            eulerOffset.y += float(dx)*_rotationSpeed;
            float x = eulerOffset.x;
            eulerOffset.x = std::min(std::max(x - float(-dy)*_rotationSpeed, -85.0f), 85.0f);
            glm::vec3 euler = this->gameObject->transform.rotation.ToEuler();
            this->gameObject->transform.rotation = Quaternion::FromEuler(eulerOffset);
        }
    });

    winMan->RegisterKeyCallback([&](int key, int action)
    {
        auto winMan = Platform::WindowManager::GetInstance();
        switch(key)
        {
            case GLFW_KEY_W:
            forward = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_A:
            left = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_S:
            backward = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_D:
            right = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_SPACE:
            up = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_LEFT_SHIFT:
            down = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_TAB:
            if(action == GLFW_PRESS)
            {
                _mouseLocked = !_mouseLocked;
                if(_mouseLocked) winMan->LockCursor(winMan->GetActiveWindow());
                else winMan->UnlockCursor(winMan->GetActiveWindow());
            }
            default:
            break;
        }
    });
    
}

void InspectorCameraComponent::Start()
{
    _rotationSpeed = 50.0f;
    _movementSpeed = 10.0f;
}
    
void InspectorCameraComponent::Update(float deltaTime)
{
    if(rigidbodycomp == nullptr) rigidbodycomp = gameObject->GetComponent<RigidBodyComponent>();
        Core::Transform* transform = &this->gameObject->transform;
        // glm::mat4 rotx = Quaternion::FromEuler(glm::vec3(transform->rotation.x, 0.0f, 0.0f)).ToMatrix();
        // glm::mat4 roty = Quaternion::FromEuler(glm::vec3(0.0f, transform->rotation.y, 0.0f)).ToMatrix();
        vec3 dir = transform->rotation*vec3(0,0,-1);
        // dir = rot*vec3(0,0,-1);
        vec3 forward = dir * _movementSpeed * deltaTime; //Negative since camera
        vec3 side = -normalize(cross(forward, vec3(0,1,0))) * _movementSpeed * deltaTime;
        vec3 up = vec3(0, 1, 0) * _movementSpeed *deltaTime;
        if(this->forward) transform->position += forward;
        if(this->left) transform->position += side;
        if(this->backward) transform->position -= forward;
        if(this->right) transform->position -= side;
        if(this->up) transform->position += up;
        if(this->down) transform->position -= up;
}

void InspectorCameraComponent::SetCamera(CameraComponent* camera)
{
    this->camera = camera;
}


void InspectorCameraComponent::DrawInspectorGUI()
{
    ImGui::DragFloat("Speed", &_movementSpeed, 0.1f, 1.0f, 500.0f);
    ImGui::Text(up ? "UP" : "DOWN");
}

} // namespace Engine::Components
