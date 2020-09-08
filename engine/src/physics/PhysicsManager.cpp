#include "physics/PhysicsManager.hpp"

#include "physics/RigidBody.hpp"

#include "renderer/Quaternion.hpp"
#include "renderer/Renderer.hpp"

#include <bullet/btBulletDynamicsCommon.h>

#include <iostream>
#include <vector>
#include <queue>

namespace Engine::Physics
{

PhysicsManager* PhysicsManager::_instance;

// BULLET ENTITIES & CONVERSION FUNCTIONS
btDiscreteDynamicsWorld *_physicsWorld;

std::vector<btRigidBody*> _btRigidbodies;
std::vector<RigidBody*> _rigidbodies;

std::queue<RBHandle> _freeHandles;

__forceinline btVector3 Convert(const glm::vec3& v) { return btVector3(v.x, v.y, v.z); }

__forceinline glm::vec3 Convert(const btVector3& v) { return glm::vec3(v.getX(), v.getY(), v.getZ()); }

__forceinline btQuaternion Convert(const Quaternion& q) { return btQuaternion(q.q.x, q.q.y, q.q.z, q.q.w); }

__forceinline Quaternion Convert(btQuaternion& q) { 
    float x = static_cast<float>(q.getX());
    float y = static_cast<float>(q.getY());
    float z = static_cast<float>(q.getZ());
    float w = static_cast<float>(q.getW());
    return Quaternion(x, y, z, w); 
}

__forceinline btTransform Convert(const Rendering::Transform& t)
{
    btTransform transform;
    transform.setOrigin(Convert(t.position));
    transform.setRotation(Convert(t.rotation));
    return transform;
}

__forceinline Rendering::Transform Convert(const btTransform& t)
{
    Rendering::Transform transform;
    transform.position = Convert(t.getOrigin());
    transform.rotation = Convert(t.getRotation());
    return transform;
}

// DEBUG DRAW
Rendering::Renderer* _renderer;

class PhysicsDebugDrawer : public btIDebugDraw
{
    DebugDrawModes mode = (DebugDrawModes)(DBG_DrawWireframe | DBG_DrawContactPoints);
    public:

    PhysicsDebugDrawer() { _renderer = Rendering::Renderer::GetInstance(); }

    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
        Rendering::LineSegment l;
        l.Width = 1;
        l.Vertices.push_back(Convert(from));
        l.Vertices.push_back(Convert(to));
        l.Colour = {color.x(), color.y(), color.z(), 1.0f};
        _renderer->DrawLineSegment(l);
    }

    void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime,
                          const btVector3 &color)
    {
        Rendering::LineSegment l;
        l.Width = 1;
        glm::vec3 pos = {PointOnB.x(), PointOnB.y(), PointOnB.z()};
        glm::vec3 end =  pos + glm::vec3(normalOnB.x(), normalOnB.y(), normalOnB.z())*3.0f;
        l.Vertices = {pos, end};
        l.Colour = {color.x(), color.y(), color.z(), 1.0f};
    }

    void reportErrorWarning(const char *warningString) 
    {
        std::cout << warningString << std::endl;
    }

    void draw3dText(const btVector3 &location, const char *textString)
    {

    }

    void setDebugMode(int debugMode)
    {
        mode = static_cast<DebugDrawModes>(debugMode);
    }

    int getDebugMode() const 
    {
        return mode;
    }
};

PhysicsDebugDrawer _debugDrawer;

btDiscreteDynamicsWorld* CreatePhysicsWorld()
{
    auto Broadphase = new btDbvtBroadphase();
    auto CollisionConfiguration = new btDefaultCollisionConfiguration();
    auto CollisionDispatcher = new btCollisionDispatcher(CollisionConfiguration);
    auto ConstraintSolver = new btSequentialImpulseConstraintSolver();

    auto world = new btDiscreteDynamicsWorld(
        CollisionDispatcher,
        Broadphase,
        ConstraintSolver,
        CollisionConfiguration);
    return world;
}

btCollisionShape* GetCollisionShape(ColliderInfo& col)
{
    switch (col.Type)
    {
    case ColliderType::BOX:
        return new btBoxShape(Convert(col.Box.HalfExtents));
        break;
    case ColliderType::CAPSULE:
        return new btCapsuleShape(col.Capsule.Radius, col.Capsule.Height);
        break;
    case ColliderType::CYLINDER:
        return new btCylinderShape(Convert(col.Cyllinder.HalfExtents));
        break;
    case ColliderType::PLANE:
        return new btStaticPlaneShape(Convert(col.Plane.N), col.Plane.D);
        break;
    case ColliderType::SPHERE:
        return new btSphereShape(col.Sphere.Radius);
        break;
    default:
        throw std::exception("Unknown collider type");
        break;
    }
}

PhysicsManager::PhysicsManager()
{
    _instance = this;
    _physicsWorld = CreatePhysicsWorld();
    
    _physicsWorld->setDebugDrawer(&_debugDrawer);
    SetGravity({0.0f, -9.82f, 0.0f});
}

PhysicsManager::~PhysicsManager()
{
    delete _physicsWorld;
}

void PhysicsManager::SetDebugDraw(bool enabled)
{
    _debugDrawEnabled = enabled;
}

PhysicsManager* PhysicsManager::GetInstance()
{
    if(_instance == nullptr) _instance = new PhysicsManager();
    return _instance;
}

void PhysicsManager::Step(float deltaTime)
{
    _physicsWorld->stepSimulation(deltaTime, 0, 1.0f / 120.0f);
}

void PhysicsManager::Update(float deltaTime)
{
    Step(deltaTime);
    SynchonizeTransforms();
    if(_debugDrawEnabled) _physicsWorld->debugDrawWorld();
}

RigidBody* PhysicsManager::CreateRigidBody(Rendering::Transform &transform, std::vector<ColliderInfo> colliders, float mass, void* owner)
{   
    btMotionState* state = new btDefaultMotionState();
    btCompoundShape* shape = new btCompoundShape();
    for(uint32_t colliderIndex = 0; colliderIndex < colliders.size(); colliderIndex++)
    {
        ColliderInfo& col = colliders[colliderIndex];
        btTransform childTrans;
        childTrans.setOrigin({0,0,0});
        childTrans.setRotation(btQuaternion(0,0,0,1));
        shape->addChildShape(childTrans, GetCollisionShape(col));
    }
    btVector3 inertia(0,0,0);
    shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo info(mass, state, shape, inertia);
    btRigidBody *bulletRigidBody = new btRigidBody(info);
    bulletRigidBody->setUserPointer(owner);
    bulletRigidBody->setWorldTransform(Convert(transform));
    _physicsWorld->addRigidBody(bulletRigidBody);
    RBHandle handle;
    RigidBody* rb;
    if(_freeHandles.size() > 0)
    {
        handle = _freeHandles.front();
        rb = new RigidBody(handle, transform);
        _btRigidbodies[handle] = bulletRigidBody;
        _rigidbodies[handle] = rb;
        _freeHandles.pop();
    }
    else
    {
        handle = RBHandle(_btRigidbodies.size());
        rb = new RigidBody(handle, transform);
        _btRigidbodies.push_back(bulletRigidBody);
        _rigidbodies.push_back(rb);
    }
    return rb;
}

void PhysicsManager::RemoveRigidBody(RigidBody* rb)
{
    RBHandle handle = rb->_handle;
    _physicsWorld->removeRigidBody(_btRigidbodies[handle]); //TODO: Double check if the object is deleted as well
    _btRigidbodies[handle] = nullptr;
    _rigidbodies[handle] = nullptr;
    _freeHandles.push(handle);
    delete rb;
}

glm::vec3 PhysicsManager::GetGravity()
{
    return Convert(_physicsWorld->getGravity());
}

void PhysicsManager::SetGravity(glm::vec3 gravity)
{
    _physicsWorld->setGravity({gravity.x, gravity.y, gravity.z});
}

void PhysicsManager::SynchonizeTransforms()
{
    for(RBHandle h = 0; h < _rigidbodies.size(); h++)
    {
        if(_rigidbodies[h] == nullptr) continue;
        // UPDATE GAME WORLD TRANSFORM
        RigidBody* rb = _rigidbodies[h];
        btRigidBody* btrb = _btRigidbodies[h];
        int32_t flags = btrb->getCollisionFlags();
        flags = rb->IsKinematic() ? flags | btrb->CF_KINEMATIC_OBJECT : flags & ~(1UL << btrb->CF_KINEMATIC_OBJECT);
        flags = rb->IsStatic() ? flags | btrb->CF_STATIC_OBJECT : flags & ~(1UL << btrb->CF_STATIC_OBJECT);
        btrb->setCollisionFlags(flags);

        btTransform& btTrans = btrb->getWorldTransform();
        glm::vec3 btPos = Convert(btTrans.getOrigin());
        Quaternion btRot = Convert(btTrans.getRotation());
        glm::vec3 deltaPos = btPos - rb->_previousTransform.position;
        Quaternion deltaRot = btRot * rb->_previousTransform.rotation.Inverse();
        rb->_transform->position += deltaPos;
        rb->_transform->rotation = rb->_transform->rotation * deltaRot;

        btTrans.setOrigin(Convert(rb->_transform->position));
        btTrans.setRotation(Convert(rb->_transform->rotation));
        btrb->setWorldTransform(btTrans);
        // TODO: update physics world as well
        rb->_previousTransform = Convert(btrb->getWorldTransform());
    }
}

void PhysicsManager::SetLinearVelocity(RigidBody* rb, glm::vec3 vel)
{
    auto btrb = _btRigidbodies[rb->_handle];
    btrb->activate();
    btrb->setLinearVelocity(Convert(vel));
}

void PhysicsManager::AddForce(RigidBody* rb, glm::vec3 force, glm::vec3 relPos)
{
    auto btrb = _btRigidbodies[rb->_handle];
    btrb->applyForce(Convert(force), Convert(relPos));
}

void PhysicsManager::AddTorque(RigidBody* rb, glm::vec3 torque)
{
    auto btrb = _btRigidbodies[rb->_handle];
    btrb->applyTorque(Convert(torque));
}

void PhysicsManager::ClearForces(RigidBody* rb)
{
    auto btrb = _btRigidbodies[rb->_handle];
    btrb->setLinearVelocity({0, 0, 0});
    btrb->setAngularVelocity({0, 0, 0});
}

void PhysicsManager::SetAngularFactor(RigidBody* rb, glm::vec3 fac)
{
    auto btrb = _btRigidbodies[rb->_handle];
    btrb->setAngularFactor(Convert(fac));
}

glm::vec3 PhysicsManager::GetAngularFactor(RigidBody* rb)
{
    return Convert(_btRigidbodies[rb->_handle]->getAngularFactor());
}

void PhysicsManager::SetLinearFactor(RigidBody* rb, glm::vec3 fac)
{
    auto btrb = _btRigidbodies[rb->_handle];
    btrb->setLinearFactor(Convert(fac));
}

glm::vec3 PhysicsManager::GetLinearFactor(RigidBody* rb)
{
    return Convert(_btRigidbodies[rb->_handle]->getLinearFactor());
}

void PhysicsManager::SetMass(RigidBody* rb, float mass, glm::vec3 inertia)
{
    auto btrb = _btRigidbodies[rb->_handle];
    btrb->setMassProps(mass, Convert(inertia));
}


}