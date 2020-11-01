#pragma once

#include "EngineTypedefs.hpp"
#include "components/BaseComponent.hpp"
#include "physics/PhysicsManager.hpp"
#include "physics/RigidBody.hpp"
#include "physics/Collision.hpp"
#include <queue>

namespace Engine::Geometry
{
    class Volume;
}


namespace Engine::Components
{

class RigidBodyComponent : public BaseComponent
{
private:
    std::queue<Engine::Physics::ContactInfo> _contacts;

    Engine::Physics::RigidBody* _rigidBody;

    bool _receiveCollisionCallbacks = true;
    bool _debugDraw = false;
protected:
public:
    
    RigidBodyComponent() {}

    void Start() override;

    void Initialize(Engine::Physics::ColliderInfo& colInfo, float mass);

    __forceinline void RegisterConctact(Engine::Physics::ContactInfo& info) { _contacts.push(info); }

    __forceinline Engine::Physics::RigidBody& GetRigidBody() { return *_rigidBody; }
    
    __forceinline bool ReceivesCollisionCallbacks() { return _receiveCollisionCallbacks; }

    void Update(float deltaTime) override;

    void DrawInspectorGUI() override;

    std::shared_ptr<Utilities::JSON::JSONValue> Serialise() override;

    void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) override;

};

    
} // namespace Engine::Components
