#include "components/RigidBodyComponent.hpp"
#include "core/GameObject.hpp"

#include <imgui.h>

#include <iostream>

using namespace std;

namespace Engine::Components
{
Engine::Physics::PhysicsManager* _physicsManager;

void RigidBodyComponent::Initialize(Engine::Physics::ColliderInfo& colInfo, float mass)
{
    colInfo.Transform = gameObject->transform;
    if(_physicsManager == nullptr) _physicsManager = Engine::Physics::PhysicsManager::GetInstance();
    _rigidBody = _physicsManager->CreateRigidBody(gameObject->transform, { colInfo } , 1.0f, this);
}

void RigidBodyComponent::Start()
{

}

void RigidBodyComponent::Update(float deltaTime)
{
    while(!_contacts.empty())
    {
        auto& c = _contacts.front();
        cout << "Contact with: " << c.other->gameObject->Name << endl;
        cout << "New : " << (c.NewCollision ? "Yes" : "No") << endl;
        cout << "Ended: " << (c.EndCollision ? "Yes" : "No") << endl;
        _contacts.pop();
    }
}

void RigidBodyComponent::DrawInspectorGUI()
{
    bool draw = _debugDraw;
    bool kinematic = _rigidBody->IsKinematic();
    bool stc = _rigidBody->IsStatic();
    ImGui::Checkbox("Draw Collider", &_debugDraw);
    ImGui::Checkbox("Kinematic", &kinematic);
    ImGui::Checkbox("Static", &stc);
    if(_debugDraw != draw)
    {
        _rigidBody->SetDrawDebugLines(_debugDraw);
    }
    if(kinematic != _rigidBody->IsKinematic())
    {
        _rigidBody->SetKinematic(kinematic);
    }
    if(stc != _rigidBody->IsStatic())
    {
        _rigidBody->SetStatic(stc);
    }  
}

}