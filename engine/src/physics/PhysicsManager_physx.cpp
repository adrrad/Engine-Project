#ifdef USE_PHYSX 

#include "physics/PhysicsManager.hpp"

#include "physics/Collision.hpp"
#include "physics/RigidBody.hpp"

#include "renderer/Quaternion.hpp"
#include "renderer/Renderer.hpp"

#include "components/RigidBodyComponent.hpp"
#include <iostream>
#include <vector>
#include <queue>

#include <physx/PxPhysicsAPI.h>


#define PVD_HOST "127.0.0.1"

using namespace physx;

namespace Engine::Physics
{


PhysicsManager* PhysicsManager::_instance;
Rendering::Renderer* _renderer;

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

std::vector<PxRigidDynamic*> _pxRigidbodies;
std::vector<RigidBody*> _rigidbodies;

std::queue<RBHandle> _freeHandles;


_forceinline PxVec3 Convert(glm::vec3 v) { return {v.x, v.y, v.z}; }

_forceinline glm::vec3 Convert(PxVec3 v) { return {v.x, v.y, v.z}; }

_forceinline PxQuat Convert(Quaternion q) { return {q.x, q.y, q.z, q.w}; }

_forceinline Quaternion Convert(PxQuat q) { return {q.x, q.y, q.z, q.w}; }

_forceinline PxTransform Convert(Rendering::Transform t) 
{ 
    PxTransform trans;
    trans.p = Convert(t.position);
    trans.q = Convert(t.rotation);
    return trans;
}

_forceinline Rendering::Transform Convert(PxTransform t) 
{ 
    Rendering::Transform trans;
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
    // mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(scale));
    // if (!mCooking)
    //     fatalError("PxCreateCooking failed!");

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
        throw "Terrain heightfield not implemented yet!";
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
    }
    default:
        throw std::exception("Unknown collider type");
        break;
    }
    PxShape* shape = mPhysics->createShape(*geometry, *mat, true);
    return shape;
}

PhysicsManager::PhysicsManager()
{
    _instance = this;
    InitializePhysX();
}

void PhysicsManager::Draw()
{
    static Rendering::Renderer* renderer = Rendering::Renderer::GetInstance();
    const PxRenderBuffer& rb = mScene->getRenderBuffer();
    for(PxU32 i=0; i < rb.getNbLines(); i++)
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
    _debugDrawEnabled = enabled;
    if(enabled) mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
    else mScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, -1.0f);
}

PhysicsManager* PhysicsManager::GetInstance()
{
    if(_instance == nullptr) return new PhysicsManager();
    return _instance;
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
    if(_debugDrawEnabled) Draw();

    // return true;

}

void PhysicsManager::Update(float deltaTime)
{
    Step(deltaTime);
    
    
    // if(_debugDrawEnabled) 
    //     _physicsWorld->debugDrawWorld();
}

RigidBody* PhysicsManager::CreateRigidBody(Rendering::Transform &transform, std::vector<ColliderInfo> colliders, float mass, void* owner)
{   
    PxShape* shape = GetCollisionShape(colliders[0]);
    PxRigidDynamic* actor = mPhysics->createRigidDynamic(Convert(transform));
    actor->attachShape(*shape);
    shape->release();
    mScene->addActor(*actor);
    RBHandle handle;
    RigidBody* rb;
    if(_freeHandles.size() > 0)
    {
        handle = _freeHandles.front();
        rb = new RigidBody(handle, transform);
        _pxRigidbodies[handle] = actor;
        _rigidbodies[handle] = rb;
        _freeHandles.pop();
    }
    else
    {
        handle = RBHandle(_pxRigidbodies.size());
        rb = new RigidBody(handle, transform);
        _pxRigidbodies.push_back(actor);
        _rigidbodies.push_back(rb);
    }
    return rb;
}

void PhysicsManager::RemoveRigidBody(RigidBody* rb)
{
    RBHandle handle = rb->_handle;
    mScene->removeActor(*_pxRigidbodies[handle]); //TODO: Double check if the object is deleted as well
    _pxRigidbodies[handle]->release();
    _pxRigidbodies[handle] = nullptr;
    _rigidbodies[handle] = nullptr;
    _freeHandles.push(handle);
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
    for(RBHandle h = 0; h < _rigidbodies.size(); h++)
    {
        if(_rigidbodies[h] == nullptr) continue;
        // UPDATE GAME WORLD TRANSFORM
        RigidBody* rb = _rigidbodies[h];
        PxRigidDynamic* pxrb = _pxRigidbodies[h];
        PxTransform& pxTrans = pxrb->getGlobalPose();
        auto grav = mScene->getGravity();
        uint32_t num = 0;
        mScene->getActiveActors(num);
        int i = 0;
        //If kinematic, only update physics world (game world has full control)
        if(rb->IsKinematic())
        {
            pxrb->wakeUp();
            pxTrans.p = Convert(rb->_transform->GetGlobalPosition());
            pxTrans.q = Convert(rb->_transform->GetGlobalRotation());
            pxrb->setKinematicTarget(pxTrans);
            continue;
        }
        // Get current transform from the physics world
        glm::vec3 btPos = Convert(pxTrans.p);
        Quaternion btRot = Convert(pxTrans.q);
        // Calculate offset from previous transform to the current one
        glm::vec3 deltaPos = btPos - rb->_previousTransform.GetGlobalPosition();
        Quaternion deltaRot = btRot * rb->_previousTransform.GetGlobalRotation().Inverse();
        // Calculate the new global transform
        glm::vec3 globalPosition = rb->_transform->GetGlobalPosition() + deltaPos;
        Quaternion globalRotation = rb->_transform->GetGlobalRotation() * deltaRot;
        // Update game world
        rb->_transform->SetGlobalPosition(globalPosition);
        rb->_transform->SetGlobalRotation(globalRotation);
        // Update physics world
        pxTrans.p = Convert(globalPosition);
        pxTrans.q = Convert(globalRotation);

        pxrb->setGlobalPose(pxTrans);
        rb->_previousTransform = Convert(pxTrans);
    }
}

void PhysicsManager::SetDebugDraw(RigidBody* rb, bool enabled)
{
    // auto* pxrb = _pxRigidbodies[rb->_handle];
    // auto flags = pxrb->getCollisionFlags();
    // flags = enabled ? flags & ~(pxrb->CF_DISABLE_VISUALIZE_OBJECT) : flags | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
    // pxrb->setCollisionFlags(flags);
    // flags = pxrb->getCollisionFlags();
}

void PhysicsManager::SetKinematic(RigidBody* rb, bool enabled)
{
    auto pxrb = _pxRigidbodies[rb->_handle];
    pxrb->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, enabled);
}

void PhysicsManager::SetStatic(RigidBody* rb, bool enabled)
{
    // auto pxrb = _pxRigidbodies[rb->_handle];
    // pxrb->activate();
    // int32_t flags = pxrb->getCollisionFlags();
    // flags = enabled ? flags | pxrb->CF_STATIC_OBJECT : flags & ~(1UL << pxrb->CF_STATIC_OBJECT);
    // pxrb->setCollisionFlags(flags);
}


void PhysicsManager::SetLinearVelocity(RigidBody* rb, glm::vec3 vel)
{
    auto pxrb = _pxRigidbodies[rb->_handle];
    pxrb->setLinearVelocity(Convert(vel));
}

void PhysicsManager::AddForce(RigidBody* rb, glm::vec3 force, glm::vec3 relPos)
{
    auto pxrb = _pxRigidbodies[rb->_handle];
    pxrb->addForce(Convert(force));
}

void PhysicsManager::AddTorque(RigidBody* rb, glm::vec3 torque)
{
    auto pxrb = _pxRigidbodies[rb->_handle];
    pxrb->addTorque(Convert(torque));
}

void PhysicsManager::ClearForces(RigidBody* rb)
{
    auto pxrb = _pxRigidbodies[rb->_handle];
    pxrb->setLinearVelocity({0, 0, 0});
    pxrb->setAngularVelocity({0, 0, 0});
}

void PhysicsManager::SetAngularFactor(RigidBody* rb, glm::vec3 fac)
{
    // auto pxrb = _pxRigidbodies[rb->_handle];
    // pxrb->setAngularFactor(Convert(fac));
}

glm::vec3 PhysicsManager::GetAngularFactor(RigidBody* rb)
{
    // return Convert(_pxRigidbodies[rb->_handle]->getAngularFactor());
    return {0,0,0};
}

void PhysicsManager::SetLinearFactor(RigidBody* rb, glm::vec3 fac)
{
    // auto pxrb = _pxRigidbodies[rb->_handle];
    // pxrb->setLinearFactor(Convert(fac));
}

glm::vec3 PhysicsManager::GetLinearFactor(RigidBody* rb)
{
    // return Convert(_pxRigidbodies[rb->_handle]->getLinearFactor());
    return {0,0,0};
}

void PhysicsManager::SetMass(RigidBody* rb, float mass, glm::vec3 inertia)
{
//     auto pxrb = _pxRigidbodies[rb->_handle];
//     pxrb->setMassProps(mass, Convert(inertia));
}


}

#endif