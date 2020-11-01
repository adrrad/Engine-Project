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

std::shared_ptr<Utilities::JSON::JSONValue> RigidBodyComponent::Serialise()
{
    using namespace Utilities::JSON;
    auto json = BaseComponent::Serialise();
    auto serialiseVec3 = [&](glm::vec3 v)
    {
        return JSONValue::AsArray({ JSONValue::AsFloat(v.x), JSONValue::AsFloat(v.y), JSONValue::AsFloat(v.z) });
    };
    auto& rb = GetRigidBody();
    auto col = rb.GetColliderInfo();
    json->Members.push_back({ "mass", JSONValue::AsFloat(rb.GetMass()) });
    json->Members.push_back({ "kinematic", JSONValue::AsBoolean(rb.IsKinematic()) });
    json->Members.push_back({ "static", JSONValue::AsBoolean(rb.IsStatic()) });
    json->Members.push_back({ "linearVelocity", serialiseVec3(rb.GetLinearVelocity()) });
    json->Members.push_back({ "linearConstraints", serialiseVec3(rb.GetLinearConstraints()) });
    json->Members.push_back({ "angularConstraints", serialiseVec3(rb.GetAngularConstraints()) });
    json->Members.push_back({ "colliderType", JSONValue::AsFloat(float(col.Type)) });

    switch (col.Type)
    {
    case Physics::ColliderType::BOX:
    case Physics::ColliderType::CYLINDER:
        json->Members.push_back({ "halfExtents", serialiseVec3(col.Box.HalfExtents) });
        break;
    case Physics::ColliderType::SPHERE:
        json->Members.push_back({ "radius", JSONValue::AsFloat(col.Sphere.Radius)});
        break;
    case Physics::ColliderType::PLANE:
        json->Members.push_back({ "normal", serialiseVec3(col.Plane.N)});
        json->Members.push_back({ "d", JSONValue::AsFloat(col.Plane.D)});
    case Physics::ColliderType::CAPSULE:
        json->Members.push_back({ "radius", JSONValue::AsFloat(float(col.Capsule.Radius)) });
        json->Members.push_back({ "height", JSONValue::AsFloat(float(col.Capsule.Height)) });
    default:
        break;
    }

    return json;
}

void RigidBodyComponent::Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    BaseComponent::Deserialise(json);
    auto velocity = json->GetMember("linearVelocity")->Array;
    auto linearConstraints = json->GetMember("linearConstraints")->Array;
    auto angularConstraints = json->GetMember("angularConstraints")->Array;
    Physics::ColliderInfo col;

    col.Type = Physics::ColliderType(json->GetMember("colliderType")->Float);
    switch (col.Type)
    {
    case Physics::ColliderType::BOX:
    case Physics::ColliderType::CYLINDER:
    {
        auto ext = json->GetMember("halfExtents")->Array;
        col.Box.HalfExtents = { ext[0]->Float, ext[1]->Float, ext[2]->Float };
        break;
    }
    case Physics::ColliderType::SPHERE:
    {
        col.Sphere.Radius = json->GetMember("radius")->Float;
        break;
    }
    case Physics::ColliderType::PLANE:
    {
        auto n = json->GetMember("normal")->Array;
        auto d = json->GetMember("d")->Float;
        col.Plane.N = { n[0]->Float, n[1]->Float, n[2]->Float };
        col.Plane.D = d;
        break;
    }
    case Physics::ColliderType::CAPSULE:
    {
        col.Capsule.Radius = json->GetMember("radius")->Float;
        col.Capsule.Height = json->GetMember("height")->Float;
    }
    default:
        break;
    }
    Initialize(col, json->GetMember("mass")->Float);
    auto& rb = GetRigidBody();
    rb.SetKinematic(json->GetMember("kinematic")->Boolean);
    rb.SetStatic(json->GetMember("static")->Boolean);
    rb.SetLinearVelocity({velocity[0]->Float, velocity[1]->Float, velocity[2]->Float});
    rb.SetLinearConstraints({linearConstraints[0]->Float, linearConstraints[1]->Float, linearConstraints[2]->Float});
    rb.SetAngularConstraints({angularConstraints[0]->Float, angularConstraints[1]->Float, angularConstraints[2]->Float});
}

}