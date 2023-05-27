#include "ECSSceneGraphSystem.hpp"
#include "components/ECSComponents.hpp"
#include "acceleration/Octree.hpp"
#include "../ECSManager.hpp"

using namespace Acceleration;

namespace Gameplay
{
    ECSSceneGraphSystem::ECSSceneGraphSystem() 
    {
        Initialise();
    }
    
    void ECSSceneGraphSystem::Update(float deltaTime)
    {
        
    }

    Signature ECSSceneGraphSystem::GetSystemSignature() 
    {
        ECSManager* ecs = ECSManager::GetInstance();
        Signature signature;
        signature.set(ecs->GetComponentType<ECS::TransformComponent>());
        signature.set(ecs->GetComponentType<ECS::AxisAlignedBoxComponent>());
        return signature;
    }
    
    void ECSSceneGraphSystem::Initialise() 
    {
        // m_sceneGraph = new Octree();
    }
    
    void ECSSceneGraphSystem::UpdateSceneGraph() 
    {
        
    }
}