#ifdef USE_PHYSX 

#include "physics/PhysicsManager.hpp"

#include "physics/Collision.hpp"
#include "physics/RigidBody.hpp"

#include "rendering/Quaternion.hpp"
#include "rendering/Renderer.hpp"

#include "components/RigidBodyComponent.hpp"

#include "utilities/MathUtils.hpp"

#include <iostream>
#include <vector>
#include <queue>

#include <physx/PxPhysicsAPI.h>


#define PVD_HOST "127.0.0.1"

using namespace physx;

namespace Engine::Physics
{

PhysicsManager* PhysicsManager::m_instance;
Rendering::Renderer* m_renderer;

// PHYSX ENTITIES & CONVERSION FUNCTIONS
static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static PxFoundation* mFoundation;
static PxPhysics* mPhysics;
static PxPvd* mPvd;
static PxPvdTransport* transport;
static PxCooking* mCooking;
static PxScene* mScene;
bool recordMemoryAllocations = true;

float mAccumulator = 0.0f;
float mStepSize = 1.0f / 60.0f;

std::vector<PxRigidDynamic*> m_pxRigidbodies;
std::vector<RigidBody*> m_rigidbodies;

std::queue<RBHandle> m_freeHandles;


_forceinline PxVec3 Convert(glm::vec3 v) { return {v.x, v.y, v.z}; }

_forceinline glm::vec3 Convert(PxVec3 v) { return {v.x, v.y, v.z}; }

_forceinline PxQuat Convert(Quaternion q) { return {q.x, q.y, q.z, q.w}; }

_forceinline Quaternion Convert(PxQuat q) { return {q.x, q.y, q.z, q.w}; }

_forceinline PxTransform Convert(Core::Transform t) 
{ 
    PxTransform trans;
    trans.p = Convert(t.position);
    trans.q = Convert(t.rotation);
    return trans;
}

_forceinline Core::Transform Convert(PxTransform t) 
{ 
    Core::Transform trans;
    trans.position = Convert(t.p);
    trans.rotation = Convert(t.q);
    return trans;
}

void fatalError(const char* msg) { throw msg; }

static void InitializePhysX()
{
    // FOUNDATION
    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
    if(!mFoundation) 
        fatalError("PxCreateFoundation failed!");
    // PHYSICS
    mPvd = PxCreatePvd(*mFoundation);
    transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
    mPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);
    auto toleranceScale = PxTolerancesScale();
    mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
        toleranceScale, recordMemoryAllocations, mPvd);
    if(!mPhysics) 
        fatalError("PxCreatePhysics failed!");
    // COOKING: utilities for creating, converting, and serializing bulk data
    mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(toleranceScale));
    if (!mCooking)
        fatalError("PxCreateCooking failed!");

    //EXTENSIONS: contains many functions that may be useful
    if (!PxInitExtensions(*mPhysics, mPvd))
        fatalError("PxInitExtensions failed!");
    PxSceneDesc desc = PxSceneDesc(mPhysics->getTolerancesScale());
    desc.gravity = {0.0f,-9.81f, 0.0f};

    desc.setToDefault(toleranceScale);
    desc.filterShader = PxDefaultSimulationFilterShader;
    auto cpudispatcher = PxDefaultCpuDispatcherCreate(4);
    desc.cpuDispatcher = cpudispatcher;
    desc.isValid();
    mScene = mPhysics->createScene(desc);
    mScene->setGravity({0, -9.82, 0});
    mScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
}


PxShape* GetCollisionShape(ColliderInfo& col)
{
    PxGeometry* geometry;
    PxMaterial* mat = mPhysics->createMaterial(0.5f, 0.5f, 0.1f);
    switch (col.Type)
    {
    case ColliderType::BOX:
        geometry = new PxBoxGeometry(Convert(col.Box.HalfExtents));
        break;
    case ColliderType::CAPSULE:
        geometry = new PxCapsuleGeometry(col.Capsule.Radius, col.Capsule.Height*0.5f);
        break;
    case ColliderType::CYLINDER:
        throw "Cylinder shape not supported!";
        break;
    case ColliderType::PLANE:
        geometry = new PxPlaneGeometry();
    case ColliderType::SPHERE:
        geometry = new PxSphereGeometry(col.Sphere.Radius);
        break;
    case ColliderType::TERRAIN:
    {
        auto terrain = col.Terrain;
        PxHeightFieldSample* samples = (PxHeightFieldSample*)malloc(sizeof(PxHeightFieldSample)*terrain.Columns*terrain.Rows);// new PxHeightFieldSample[terrain.Columns*terrain.Rows];
        // auto v = m_aligned_malloc(32, 16);
        for(int x = 0; x < terrain.Columns; x++)
        {
            for(int y = 0; y < terrain.Rows; y++)
            {
                // PxI16 h = 
                samples[x+y*terrain.Columns].height = (PxI16)(terrain.Data[y+x*terrain.Columns]/terrain.HeightScale);
                samples[x+y*terrain.Columns].setTessFlag();
                samples[x+y*terrain.Columns].materialIndex0=1;
                samples[x+y*terrain.Columns].materialIndex1=1;
            }
        }
        PxHeightFieldDesc hfDesc;
        hfDesc.format = PxHeightFieldFormat::eS16_TM;
        hfDesc.nbColumns = terrain.Columns;
        hfDesc.nbRows = terrain.Rows;
        hfDesc.samples.data = samples;
        hfDesc.samples.stride = sizeof(PxHeightFieldSample);
        PxHeightField* heightField = mCooking->createHeightField(hfDesc, mPhysics->getPhysicsInsertionCallback());
        if(!heightField)
		    fatalError("creating the heightfield failed");
        geometry = new PxHeightFieldGeometry(heightField, PxMeshGeometryFlags(), terrain.HeightScale, col.LocalScaling.x, col.LocalScaling.z);
        // PxHeightField* hf = mPhysics->createHeightField()
        // PxHeightField* hf = mPhysics->createHeightField()
        // geometry = PxHeightFieldGeometry()
        // auto& ter = col.Terrain;
        // auto hf = new btHeightfieldTerrainShape(
        //                         ter.Columns, 
        //                         ter.Rows, 
        //                         ter.Data, 
        //                         ter.HeightScale, 
        //                         ter.MinHeight, 
        //                         ter.MaxHeight, 
        //                         1, 
        //                         PHY_FLOAT, 
        //                         true);
        // hf->setUseDiamondSubdivision(true);
        // hf->buildAccelerator();  
        // return hf;
        break;
    }
    default:
        throw std::exception("Unknown collider type");
        break;
    }
    PxShape* shape = mPhysics->createShape(*geometry, *mat, true);
    shape->setFlag(PxShapeFlag::eVISUALIZATION, false);
    return shape;
}

PhysicsManager::PhysicsManager()
{
    m_instance = this;
    InitializePhysX();
}

void PhysicsManager::Draw()
{
    static Rendering::Renderer* renderer = Rendering::Renderer::GetInstance();
    const PxRenderBuffer& rb = mScene->getRenderBuffer();
    for(PxU32 i=0; i < rb.getNbLines(); i+=2)
    {
        const PxDebugLine& line = rb.getLines()[i];
        Rendering::LineSegment l;
        l.Vertices = { Convert(line.pos0), Convert(line.pos1) };
        l.Colour = {0,1,0,1};
        renderer->DrawLineSegment(l);
    }
}

PhysicsManager::~PhysicsManager()
{
    mPhysics->release();
    mFoundation->release();
}

void PhysicsManager::SetDebugDraw(bool enabled)
{
    m_debugDrawEnabled = enabled;
    if(enabled) mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
    else mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, -1.0f);
}

PhysicsManager* PhysicsManager::GetInstance()
{
    if(m_instance == nullptr) return new PhysicsManager();
    return m_instance;
}

void PhysicsManager::Step(float deltaTime)
{
    mAccumulator += deltaTime;
    if(mAccumulator < mStepSize)
        return;
    mAccumulator -= mStepSize;
    mScene->simulate(mStepSize);
    bool results_fetched = mScene->fetchResults(true);
    SynchonizeTransforms();
    if(m_debugDrawEnabled) Draw();

    // return true;

}

void PhysicsManager::Update(float deltaTime)
{
    Step(deltaTime);
    
    
    // if(m_debugDrawEnabled) 
    //     m_physicsWorld->debugDrawWorld();
}

RigidBody* PhysicsManager::CreateRigidBody(Core::Transform &transform, std::vector<ColliderInfo> colliders, float mass, void* owner)
{   
    PxShape* shape = GetCollisionShape(colliders[0]);
    PxRigidDynamic* actor = mPhysics->createRigidDynamic(Convert(transform));
    if(colliders[0].Type == Engine::Physics::ColliderType::TERRAIN) actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
    actor->setActorFlag(PxActorFlag::eVISUALIZATION, false);
    actor->attachShape(*shape);
    shape->release();
    mScene->addActor(*actor);
    RBHandle handle;
    RigidBody* rb;
    if(m_freeHandles.size() > 0)
    {
        handle = m_freeHandles.front();
        rb = new RigidBody(handle, transform, colliders[0]);
        m_pxRigidbodies[handle] = actor;
        m_rigidbodies[handle] = rb;
        m_freeHandles.pop();
    }
    else
    {
        handle = RBHandle(m_pxRigidbodies.size());
        rb = new RigidBody(handle, transform, colliders[0]);
        m_pxRigidbodies.push_back(actor);
        m_rigidbodies.push_back(rb);
    }
    return rb;
}

void PhysicsManager::RemoveRigidBody(RigidBody* rb)
{
    RBHandle handle = rb->m_handle;
    mScene->removeActor(*m_pxRigidbodies[handle]); //TODO: Double check if the object is deleted as well
    m_pxRigidbodies[handle]->release();
    m_pxRigidbodies[handle] = nullptr;
    m_rigidbodies[handle] = nullptr;
    m_freeHandles.push(handle);
    delete rb;
}

glm::vec3 PhysicsManager::GetGravity()
{
    return Convert(mScene->getGravity());
}

void PhysicsManager::SetGravity(glm::vec3 gravity)
{
    mScene->setGravity({gravity.x, gravity.y, gravity.z});
}

void PhysicsManager::SynchonizeTransforms()
{
    for(RBHandle h = 0; h < m_rigidbodies.size(); h++)
    {
        if(m_rigidbodies[h] == nullptr) continue;
        // UPDATE GAME WORLD TRANSFORM
        RigidBody* rb = m_rigidbodies[h];
        PxRigidDynamic* pxrb = m_pxRigidbodies[h];
        PxTransform pxTrans = pxrb->getGlobalPose();

        auto grav = mScene->getGravity();
        uint32_t num = 0;
        mScene->getActiveActors(num);
        int i = 0;
        //If kinematic, only update physics world (game world has full control)
        if(rb->IsKinematic())
        {
            pxTrans.p = Convert(rb->m_transform->GetGlobalPosition());
            pxTrans.q = Convert(rb->m_transform->GetGlobalRotation());
            pxrb->setKinematicTarget(pxTrans);
            continue;
        }
        // Get current transform from the physics world
        glm::vec3 btPos = Convert(pxTrans.p);
        Quaternion btRot = Convert(pxTrans.q);
        // Calculate offset from stored last frame's transform to the current one
        glm::vec3 deltaPos = btPos - rb->m_previousTransform.GetGlobalPosition();
        Quaternion deltaRot = btRot * rb->m_previousTransform.GetGlobalRotation().Inverse();
        // Get the game world global transform
        glm::vec3 globalPosition = rb->m_transform->GetGlobalPosition();
        Quaternion globalRotation = rb->m_transform->GetGlobalRotation();
        glm::vec3 prevGlobalPosition = rb->m_transform->GetGlobalPosition();
        Quaternion prevGlobalRotation = rb->m_transform->GetGlobalRotation();
        // Update the game world global transform
        globalPosition = globalPosition + deltaPos;
        globalRotation = globalRotation * deltaRot;
        // Update game world
        rb->m_transform->SetGlobalPosition(globalPosition);
        rb->m_transform->SetGlobalRotation(globalRotation);
        // Update physics world
        pxTrans.p = Convert(globalPosition);
        pxTrans.q = Convert(globalRotation);
        pxrb->setGlobalPose(pxTrans);

        rb->m_previousTransform = Convert(pxTrans);
    }
}

void PhysicsManager::SetDebugDraw(RigidBody* rb, bool enabled)
{
    auto pxrb = m_pxRigidbodies[rb->m_handle];
    pxrb->setActorFlag(PxActorFlag::eVISUALIZATION, enabled);
    //physx::PxShape **userBuffer, physx::PxU32 bufferSize, physx::PxU32 startIndex = 0U
    PxShape *pshape[10];
    auto numShapes = pxrb->getShapes(pshape, 10);
    for(uint32_t i = 0; i < numShapes; i++)
    {
        pshape[i]->setFlag(PxShapeFlag::eVISUALIZATION, enabled);
    }
}

void PhysicsManager::SetKinematic(RigidBody* rb, bool enabled)
{
    auto pxrb = m_pxRigidbodies[rb->m_handle];
    pxrb->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, enabled);
}

void PhysicsManager::SetStatic(RigidBody* rb, bool enabled)
{
    // auto pxrb = m_pxRigidbodies[rb->m_handle];
    // pxrb->activate();
    // int32_t flags = pxrb->getCollisionFlags();
    // flags = enabled ? flags | pxrb->CF_STATIC_OBJECT : flags & ~(1UL << pxrb->CF_STATIC_OBJECT);
    // pxrb->setCollisionFlags(flags);
}


void PhysicsManager::SetLinearVelocity(RigidBody* rb, glm::vec3 vel)
{
    auto pxrb = m_pxRigidbodies[rb->m_handle];
    pxrb->setLinearVelocity(Convert(vel));
}

glm::vec3 PhysicsManager::GetLinearVelocity(RigidBody* rb)
{
    return Convert(m_pxRigidbodies[rb->m_handle]->getLinearVelocity());
}

void PhysicsManager::AddForce(RigidBody* rb, glm::vec3 force)
{
    auto pxrb = m_pxRigidbodies[rb->m_handle];
    pxrb->addForce(Convert(force));
}

void PhysicsManager::AddTorque(RigidBody* rb, glm::vec3 torque)
{
    auto pxrb = m_pxRigidbodies[rb->m_handle];
    pxrb->addTorque(Convert(torque));
}

void PhysicsManager::ClearForces(RigidBody* rb)
{
    auto pxrb = m_pxRigidbodies[rb->m_handle];
    pxrb->setLinearVelocity({0, 0, 0});
    pxrb->setAngularVelocity({0, 0, 0});
}

void PhysicsManager::SetAngularConstraints(RigidBody* rb, glm::bvec3 fac)
{
    auto pxrb = m_pxRigidbodies[rb->m_handle];
    pxrb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, fac.x);
    pxrb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, fac.y);
    pxrb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, fac.z);
}

glm::bvec3 PhysicsManager::GetAngularConstraints(RigidBody* rb)
{
    auto flags = m_pxRigidbodies[rb->m_handle]->getRigidDynamicLockFlags();
    return {flags & PxRigidDynamicLockFlag::eLOCK_ANGULAR_X,
            flags & PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y,
            flags & PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z};
}

void PhysicsManager::SetLinearConstraints(RigidBody* rb, glm::bvec3 fac)
{
    auto pxrb = m_pxRigidbodies[rb->m_handle];
    pxrb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, fac.x);
    pxrb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, fac.y);
    pxrb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, fac.z);
}

glm::bvec3 PhysicsManager::GetLinearConstraints(RigidBody* rb)
{
    auto flags = m_pxRigidbodies[rb->m_handle]->getRigidDynamicLockFlags();
        return {flags & PxRigidDynamicLockFlag::eLOCK_LINEAR_X,
                flags & PxRigidDynamicLockFlag::eLOCK_LINEAR_Y,
                flags & PxRigidDynamicLockFlag::eLOCK_LINEAR_Z};
}

void PhysicsManager::SetMass(RigidBody* rb, float mass)
{
    m_pxRigidbodies[rb->m_handle]->setMass(mass);
}

float PhysicsManager::GetMass(RigidBody*rb)
{
    return m_pxRigidbodies[rb->m_handle]->getMass();
}

}

#endif