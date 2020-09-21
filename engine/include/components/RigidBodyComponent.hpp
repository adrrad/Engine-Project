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
    SERIALISABLE(RigidBodyComponent, float, DUMMY);
    std::queue<Engine::Physics::ContactInfo> _contacts;

    Engine::Physics::RigidBody* _rigidBody;

    bool _receiveCollisionCallbacks = true;
    bool _debugDraw = false;
protected:
public:
    
    RigidBodyComponent() : BaseComponent("RigidBody Component") {}

    void Start() override;

    void Initialize(Engine::Physics::ColliderInfo& colInfo, float mass);

    __forceinline void RegisterConctact(Engine::Physics::ContactInfo& info) { _contacts.push(info); }

    __forceinline Engine::Physics::RigidBody& GetRigidBody() { return *_rigidBody; }
    
    __forceinline bool ReceivesCollisionCallbacks() { return _receiveCollisionCallbacks; }

    void Update(float deltaTime) override;

    void DrawInspectorGUI() override;
    
    inline std::string GetSerialised(int indent) override
    {
        return Utilities::Serialisation::SerializeObject(this, indent);
    }
};

    
} // namespace Engine::Components
