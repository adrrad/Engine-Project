#pragma once

#include "ECSEntityManager.hpp"
#include "ECSSystemManager.hpp"
#include "ECSComponentManager.hpp"

namespace Gameplay
{
    
class ECSManager
{
public:
    
    static ECSManager* GetInstance();

    EntityID GetEntity();

    void ReleaseEntity(EntityID entity);

    template<class T>
    void RegisterComponent();

    template<class T>
    T& AddComponent(EntityID entity);

    template<class T>
    void RemoveComponent(EntityID entity);

    template<class T>
    T& GetComponent(EntityID entity);

    template<class T>
    ComponentType GetComponentType();
    
    template<class T>
    T* RegisterSystem();
    
    template<class T>
    void SetSystemSignature(Signature signature);

private:
    ECSManager();

    void Initialise();


    ECSEntityManager* m_entityManager;
    ECSComponentManager* m_componentManager;
    ECSSystemManager* m_systemManager;

    static ECSManager* s_instance;
};

template<class T>
void ECSManager::RegisterComponent() 
{
    m_componentManager->RegisterComponent<T>();
}

template<class T>
T& ECSManager::AddComponent(EntityID entity) 
{
    auto& comp = m_componentManager->AddComponent<T>(entity);
    auto signature = m_entityManager->GetSignature(entity);
    signature.set(m_componentManager->GetComponentType<T>(), true);
    m_entityManager->SetSignature(entity, signature);
    m_systemManager->EntitySignatureChanged(entity, signature);
    return comp;
}

template<class T>
void ECSManager::RemoveComponent(EntityID entity) 
{
    m_componentManager->RemoveComponent<T>(entity);
    auto signature = m_entityManager->GetSignature(entity);
    signature.set(m_componentManager->GetComponentType<T>(), false);
    m_entityManager->SetSignature(entity, signature);
    m_systemManager->EntitySignatureChanged(entity, signature);
}

template<class T>
T& ECSManager::GetComponent(EntityID entity) 
{
    return m_componentManager->GetComponent<T>(entity);
}

template<class T>
ComponentType ECSManager::GetComponentType() 
{
    return m_componentManager->GetComponentType<T>();
}

template<class T>
T* ECSManager::RegisterSystem() 
{
    return m_systemManager->RegisterSystem<T>();
}

template<class T>
void ECSManager::SetSystemSignature(Signature signature) 
{
    m_systemManager->SetSystemSignature<T>(signature);
}


} // namespace Gameplay
