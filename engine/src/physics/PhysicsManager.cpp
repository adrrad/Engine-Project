#ifdef USE_BULLET

#include "physics/PhysicsManager.hpp"

#include "physics/Collision.hpp"
#include "physics/RigidBody.hpp"

#include "rendering/Quaternion.hpp"
#include "rendering/Renderer.hpp"

#include "components/RigidBodyComponent.hpp"

#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>



#include <iostream>
#include <vector>
#include <queue>

namespace Engine::Physics
{

PhysicsManager* PhysicsManager::_instance;
Rendering::Renderer* m_renderer;


// BULLET ENTITIES & CONVERSION FUNCTIONS
btDiscreteDynamicsWorld *m_physicsWorld;

std::vector<btRigidBody*> m_btRigidbodies;
std::vector<RigidBody*> m_rigidbodies;

std::queue<RBHandle> m_freeHandles;

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

Components::RigidBodyComponent *GetPhysicsComponent(void *src)
{
    btRigidBody *rb = static_cast<btRigidBody *>(src);
    return static_cast<Components::RigidBodyComponent *>(rb->getUserPointer());
};

// BULLET CALLBACKS
static bool ContactProcessedCallback(btManifoldPoint &cp, void* bodyA, void* bodyB)
{
    auto compA = GetPhysicsComponent(bodyA);
    auto compB = GetPhysicsComponent(bodyB);

    bool collisionBegin = cp.m_userPersistentData == nullptr;
    Colliders* col = (Colliders*)cp.m_userPersistentData;

    // New collision
    if (collisionBegin)
    {
        col = new Colliders();
        col->A = compA;
        col->B = compB;
        cp.m_userPersistentData = col;
    }

    if (compA->ReceivesCollisionCallbacks())
    {
        ContactInfo info;
        info.other = compB;
        info.Point = Convert(cp.getPositionWorldOnA());
        info.Normal = Convert(cp.m_normalWorldOnB);
        info.NewCollision = collisionBegin;
        info.EndCollision = false;
        compA->RegisterConctact(info);
    }

    if (compB->ReceivesCollisionCallbacks())
    {
        ContactInfo info;
        info.other = compA;
        info.Point = Convert(cp.getPositionWorldOnA());
        info.Normal = Convert(cp.m_normalWorldOnB);
        info.NewCollision = collisionBegin;
        info.EndCollision = false;
        compB->RegisterConctact(info);
    }

    return false;
}

static bool ContactDestroyedCallback(void *data)
{
    Colliders* col = (Colliders*)data;

    ContactInfo colDataA;
    colDataA.other = col->B;
    colDataA.NewCollision = false;
    colDataA.EndCollision = true;
    ContactInfo colDataB;
    colDataB.other = col->A;
    colDataB.NewCollision = false;
    colDataB.EndCollision = true;
    col->A->RegisterConctact(colDataA);
    col->B->RegisterConctact(colDataB);
    delete col;
    return true;
}


// DEBUG DRAW
class PhysicsDebugDrawer : public btIDebugDraw
{
    DebugDrawModes mode = (DebugDrawModes)(DBG_DrawWireframe | DBG_DrawContactPoints);
    int lineIndex = 0;
    public:

    PhysicsDebugDrawer() { m_renderer = Rendering::Renderer::GetInstance(); }

    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
        if(lineIndex % 2 == 1)
        {
            lineIndex = 0;
            return;
        }
        Rendering::LineSegment l;
        l.Width = 1;
        l.Vertices.push_back(Convert(from));
        l.Vertices.push_back(Convert(to));
        l.Colour = {color.x(), color.y(), color.z(), 1.0f};
        m_renderer->DrawLineSegment(l);
        lineIndex++;
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

    void draw3dText(const btVector3 &location, const char *textString) {}

    void setDebugMode(int debugMode)
    {
        mode = static_cast<DebugDrawModes>(debugMode);
    }

    int getDebugMode() const 
    {
        return mode;
    }
};

PhysicsDebugDrawer m_debugDrawer;

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
    case ColliderType::TERRAIN:
    {
        auto& ter = col.Terrain;
        auto hf = new btHeightfieldTerrainShape(
                                ter.Columns, 
                                ter.Rows, 
                                ter.Data, 
                                ter.HeightScale, 
                                ter.MinHeight, 
                                ter.MaxHeight, 
                                1, 
                                PHY_FLOAT, 
                                true);
        hf->setUseDiamondSubdivision(true);
        hf->buildAccelerator();  
        return hf;
    }
    default:
        throw std::exception("Unknown collider type");
        break;
    }
}

PhysicsManager::PhysicsManager()
{
    m_instance = this;
    m_physicsWorld = CreatePhysicsWorld();
    m_physicsWorld->setDebugDrawer(&m_debugDrawer);
    SetGravity({0.0f, -9.82f, 0.0f});
    gContactProcessedCallback = ContactProcessedCallback;
    gContactDestroyedCallback = ContactDestroyedCallback;
}

PhysicsManager::~PhysicsManager()
{
    delete m_physicsWorld;
}

void PhysicsManager::SetDebugDraw(bool enabled)
{
    m_debugDrawEnabled = enabled;
}

PhysicsManager* PhysicsManager::GetInstance()
{
    if(m_instance == nullptr) m_instance = new PhysicsManager();
    return m_instance;
}

void PhysicsManager::Step(float deltaTime)
{
    m_physicsWorld->stepSimulation(deltaTime, 0, 1.0f / 120.0f);
}

void PhysicsManager::Update(float deltaTime)
{
    Step(deltaTime);
    SynchonizeTransforms();
    if(m_debugDrawEnabled) 
        m_physicsWorld->debugDrawWorld();
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
        auto colshape = GetCollisionShape(col);

        colshape->setLocalScaling({col.LocalScaling.x, col.LocalScaling.y, col.LocalScaling.z });
        shape->addChildShape(childTrans, colshape);
    }
    btVector3 inertia(0,0,0);
    shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo info(mass, state, shape, inertia);
    info.m_friction = 10.0f;
    btRigidBody *bulletRigidBody = new btRigidBody(info);
    bulletRigidBody->setUserPointer(owner);
    bulletRigidBody->setWorldTransform(Convert(transform));
    m_physicsWorld->addRigidBody(bulletRigidBody);
    int32_t flags = bulletRigidBody->getCollisionFlags();
    flags |= btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
    bulletRigidBody->setCollisionFlags(flags);
    RBHandle handle;
    RigidBody* rb;
    if(m_freeHandles.size() > 0)
    {
        handle = m_freeHandles.front();
        rb = new RigidBody(handle, transform);
        m_btRigidbodies[handle] = bulletRigidBody;
        m_rigidbodies[handle] = rb;
        m_freeHandles.pop();
    }
    else
    {
        handle = RBHandle(m_btRigidbodies.size());
        rb = new RigidBody(handle, transform);
        m_btRigidbodies.push_back(bulletRigidBody);
        m_rigidbodies.push_back(rb);
    }
    return rb;
}

void PhysicsManager::RemoveRigidBody(RigidBody* rb)
{
    RBHandle handle = rb->m_handle;
    m_physicsWorld->removeRigidBody(m_btRigidbodies[handle]); //TODO: Double check if the object is deleted as well
    m_btRigidbodies[handle] = nullptr;
    m_rigidbodies[handle] = nullptr;
    m_freeHandles.push(handle);
    delete rb;
}

glm::vec3 PhysicsManager::GetGravity()
{
    return Convert(m_physicsWorld->getGravity());
}

void PhysicsManager::SetGravity(glm::vec3 gravity)
{
    m_physicsWorld->setGravity({gravity.x, gravity.y, gravity.z});
}

void PhysicsManager::SynchonizeTransforms()
{
    for(RBHandle h = 0; h < m_rigidbodies.size(); h++)
    {
        if(m_rigidbodies[h] == nullptr) continue;
        // UPDATE GAME WORLD TRANSFORM
        RigidBody* rb = m_rigidbodies[h];
        btRigidBody* btrb = m_btRigidbodies[h];
        btTransform& btTrans = btrb->getWorldTransform();
        //If kinematic, only update physics world (game world has full control)
        if(rb->IsKinematic())
        {
            btTrans.setOrigin(Convert(rb->m_transform->GetGlobalPosition()));
            btTrans.setRotation(Convert(rb->m_transform->GetGlobalRotation()));
            continue;
        }
        // Get current transform from the physics world
        glm::vec3 btPos = Convert(btTrans.getOrigin());
        Quaternion btRot = Convert(btTrans.getRotation());
        // Calculate offset from previous transform to the current one
        glm::vec3 deltaPos = btPos - rb->m_previousTransform.GetGlobalPosition();
        Quaternion deltaRot = btRot * rb->m_previousTransform.GetGlobalRotation().Inverse();
        // Calculate the new global transform
        glm::vec3 globalPosition = rb->m_transform->GetGlobalPosition() + deltaPos;
        Quaternion globalRotation = rb->m_transform->GetGlobalRotation() * deltaRot;
        // Update game world
        rb->m_transform->SetGlobalPosition(globalPosition);
        rb->m_transform->SetGlobalRotation(globalRotation);
        // Update physics world
        // btTrans.setOrigin(Convert(globalPosition));
        // btTrans.setRotation(Convert(globalRotation));
        // btrb->setWorldTransform(btTrans);
        rb->m_previousTransform = Convert(btrb->getWorldTransform());
    }
}

void PhysicsManager::SetDebugDraw(RigidBody* rb, bool enabled)
{
    auto* btrb = m_btRigidbodies[rb->m_handle];
    auto flags = btrb->getCollisionFlags();
    flags = enabled ? flags & ~(btrb->CF_DISABLE_VISUALIZE_OBJECT) : flags | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
    btrb->setCollisionFlags(flags);
    flags = btrb->getCollisionFlags();
}

void PhysicsManager::SetKinematic(RigidBody* rb, bool enabled)
{
    auto btrb = m_btRigidbodies[rb->m_handle];
    btrb->activate();
    int32_t flags = btrb->getCollisionFlags();
    flags = enabled ? flags | btrb->CF_KINEMATIC_OBJECT : flags & ~(1UL << btrb->CF_KINEMATIC_OBJECT);
    btrb->setCollisionFlags(flags);
}

void PhysicsManager::SetStatic(RigidBody* rb, bool enabled)
{
    auto btrb = m_btRigidbodies[rb->m_handle];
    btrb->activate();
    int32_t flags = btrb->getCollisionFlags();
    flags = enabled ? flags | btrb->CF_STATIC_OBJECT : flags & ~(1UL << btrb->CF_STATIC_OBJECT);
    btrb->setCollisionFlags(flags);
}


void PhysicsManager::SetLinearVelocity(RigidBody* rb, glm::vec3 vel)
{
    auto btrb = m_btRigidbodies[rb->m_handle];
    btrb->activate();
    btrb->setLinearVelocity(Convert(vel));
}

void PhysicsManager::AddForce(RigidBody* rb, glm::vec3 force, glm::vec3 relPos)
{
    auto btrb = m_btRigidbodies[rb->m_handle];
    btrb->applyForce(Convert(force), Convert(relPos));
}

void PhysicsManager::AddTorque(RigidBody* rb, glm::vec3 torque)
{
    auto btrb = m_btRigidbodies[rb->m_handle];
    btrb->applyTorque(Convert(torque));
}

void PhysicsManager::ClearForces(RigidBody* rb)
{
    auto btrb = m_btRigidbodies[rb->m_handle];
    btrb->setLinearVelocity({0, 0, 0});
    btrb->setAngularVelocity({0, 0, 0});
}

void PhysicsManager::SetAngularFactor(RigidBody* rb, glm::vec3 fac)
{
    auto btrb = m_btRigidbodies[rb->m_handle];
    btrb->setAngularFactor(Convert(fac));
}

glm::vec3 PhysicsManager::GetAngularFactor(RigidBody* rb)
{
    return Convert(m_btRigidbodies[rb->m_handle]->getAngularFactor());
}

void PhysicsManager::SetLinearFactor(RigidBody* rb, glm::vec3 fac)
{
    auto btrb = m_btRigidbodies[rb->m_handle];
    btrb->setLinearFactor(Convert(fac));
}

glm::vec3 PhysicsManager::GetLinearFactor(RigidBody* rb)
{
    return Convert(m_btRigidbodies[rb->m_handle]->getLinearFactor());
}

void PhysicsManager::SetMass(RigidBody* rb, float mass, glm::vec3 inertia)
{
    auto btrb = m_btRigidbodies[rb->m_handle];
    btrb->setMassProps(mass, Convert(inertia));
}


}

#endif