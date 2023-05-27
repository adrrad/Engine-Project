#include "ECSRenderingSystem.hpp"
#include "components/ECSComponents.hpp"
#include "../ECSManager.hpp"
#include "../ECSComponentManager.hpp"
#include "../ECSHelperFunctions.hpp"

using namespace Gameplay::ECS;

namespace Gameplay
{
    void ECSRenderingSystem::Update(float deltaTime)
    {
        
    }


    Signature ECSRenderingSystem::GetSystemSignature() 
    {

        ECSManager* ecs = ECSManager::GetInstance();
        Signature signature;
        signature.set(ecs->GetComponentType<TransformComponent>());
        signature.set(ecs->GetComponentType<ECS::MeshComponent>());
        signature.set(ecs->GetComponentType<MaterialComponent>());
        return signature;
    }
    
    void ECSRenderingSystem::Render(Rendering::Renderpass::RenderpassBuilder& rpb) 
    {
        for(auto entity : m_entities)
        {
            TransformComponent& t = GetComponent<TransformComponent>(entity);
            ECS::MeshComponent& m = GetComponent<ECS::MeshComponent>(entity);
            MaterialComponent& mt = GetComponent<MaterialComponent>(entity);

            rpb.UseMaterial(mt.MaterialInstance);
            rpb.DrawMesh(m.VAO, GL_TRIANGLES, m.IndexCount);
        }
    }
}