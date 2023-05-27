#include "components/InspectorCameraComponent.hpp"
#include "platform/WindowManager.hpp"
#include "gameplay/GameObject.hpp"
#include "rendering/Quaternion.hpp"
#include "components/RigidBodyComponent.hpp"
#include <imgui.h>

#include <iostream>
#include <algorithm>
#include <math.h>

using namespace glm;
using namespace std;


namespace Gameplay
{
using namespace Rendering;
InspectorCameraComponent::InspectorCameraComponent()
{

}

void InspectorCameraComponent::Start()
{
    m_rotationSpeed = 50.0f;
    m_movementSpeed = 10.0f;
        auto winMan = Platform::WindowManager::GetInstance();
    winMan->RegisterMousePositionCallback([&](double dx, double dy)
    {
        if(m_mouseLocked && Enabled())
        {
            eulerOffset.y += float(dx)*m_rotationSpeed;
            float x = eulerOffset.x;
            eulerOffset.x = std::min(std::max(x - float(-dy)*m_rotationSpeed, -85.0f), 85.0f);
            glm::vec3 euler = this->gameObject->transform.rotation.ToEuler();
            this->gameObject->transform.rotation = Quaternion::FromEuler(eulerOffset);
        }
    });
    
    winMan->RegisterKeyCallback([&](int key, int action)
    {
        if(!Enabled()) return;
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
                m_mouseLocked = !m_mouseLocked;
                if(m_mouseLocked) winMan->LockCursor(winMan->GetActiveWindow());
                else winMan->UnlockCursor(winMan->GetActiveWindow());
            }
            default:
            break;
        }
    });
    
}
    
void InspectorCameraComponent::Update(float deltaTime)
{
    if(rigidbodycomp == nullptr) rigidbodycomp = gameObject->GetComponent<RigidBodyComponent>();
        Gameplay::Transform* transform = &this->gameObject->transform;
        // glm::mat4 rotx = Quaternion::FromEuler(glm::vec3(transform->rotation.x, 0.0f, 0.0f)).ToMatrix();
        // glm::mat4 roty = Quaternion::FromEuler(glm::vec3(0.0f, transform->rotation.y, 0.0f)).ToMatrix();
        vec3 dir = transform->rotation*vec3(0,0,-1);
        // dir = rot*vec3(0,0,-1);
        vec3 forward = dir * m_movementSpeed * deltaTime; //Negative since camera
        vec3 side = -normalize(cross(forward, vec3(0,1,0))) * m_movementSpeed * deltaTime;
        vec3 up = vec3(0, 1, 0) * m_movementSpeed *deltaTime;
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

} // namespace Gameplay
