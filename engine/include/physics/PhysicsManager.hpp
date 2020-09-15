#pragma once

#include "renderer/Transform.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace Engine::Physics
{
class RigidBody;

enum class ColliderType
{
    BOX,
    CYLINDER,
    CAPSULE,
    PLANE,
    SPHERE,
    TERRAIN
};

struct ColliderInfo 
{
    Rendering::Transform Transform;
    glm::vec3 LocalScaling;
    ColliderType Type;
    union
    {
        struct
        {
            glm::vec3 HalfExtents;
        } Box, Cyllinder;

        struct
        {
            float Radius;
        } Sphere;

        struct
        {
            glm::vec3 N;
            float D;
        } Plane;
        struct
        {
            float Radius;
            float Height;
        } Capsule;
        struct
        {
            int Columns;
            int Rows;
            float* Data;
            float HeightScale;
            float MinHeight;
            float MaxHeight;
            int UpAxis;
        } Terrain;
    };

    ColliderInfo()
    {
        LocalScaling = {1, 1, 1};
    }
};

class PhysicsManager
{
    static PhysicsManager* _instance;
    PhysicsManager();

    bool _debugDrawEnabled = false;

    void Draw();

public:

    ~PhysicsManager();

    void SetDebugDraw(bool enabled);

    static PhysicsManager* GetInstance();

    void Step(float deltaTime);

    void Update(float deltaTime);

    RigidBody* CreateRigidBody(Rendering::Transform &transform, std::vector<ColliderInfo> colliders, float mass, void* owner);

    void RemoveRigidBody(RigidBody* rb);

    glm::vec3 GetGravity();

    void SetDebugDraw(RigidBody* rb, bool enabled);

    void SetKinematic(RigidBody* rb, bool enabled);

    void SetStatic(RigidBody* rb, bool enabled);

    void SetGravity(glm::vec3 gravity);

    void SynchonizeTransforms();

    void SetLinearVelocity(RigidBody* rb, glm::vec3 vel);

    glm::vec3 GetLinearVelocity(RigidBody* rb);

    void AddForce(RigidBody* rb, glm::vec3 force);

    void AddTorque(RigidBody* rb, glm::vec3 torque);

    void ClearForces(RigidBody* rb);

    void SetAngularConstraints(RigidBody* rb, glm::bvec3 fac);

    glm::bvec3 GetAngularConstraints(RigidBody* rb);

    void SetLinearConstraints(RigidBody* rb, glm::bvec3 fac);

    glm::bvec3 GetLinearConstraints(RigidBody* rb);
    
    void SetMass(RigidBody* rb, float mass);
};

} // namespace Engine::Physics
