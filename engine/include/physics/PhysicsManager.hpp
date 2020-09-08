#pragma once

#include "physics/RigidBody.hpp"
#include "renderer/Transform.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace Engine::Physics
{

enum class ColliderType
{
    BOX,
    CYLINDER,
    CAPSULE,
    PLANE,
    SPHERE
};

struct ColliderInfo 
{
    Rendering::Transform Transform;
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
    };
};

class PhysicsManager
{
    static PhysicsManager* _instance;
    PhysicsManager();

public:

    ~PhysicsManager();

    static PhysicsManager* GetInstance();

    void Step(float deltaTime);

    void Update(float deltaTime);

    RigidBody* CreateRigidBody(Rendering::Transform &transform, std::vector<ColliderInfo> colliders, float mass, void* owner);

    void RemoveRigidBody(RigidBody* rb);

    glm::vec3 GetGravity();

    void SetGravity(glm::vec3 gravity);

    void SynchonizeTransforms();
};

} // namespace Engine::Physics
