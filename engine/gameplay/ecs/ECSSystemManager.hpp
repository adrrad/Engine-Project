#pragma once

#include "ECSDefinitions.hpp"
#include "ECSSystembase.hpp"
#include "../../Debug.hpp"

#include <unordered_map>

namespace Gameplay
{

class ECSSystemManager
{
public:
    static ECSSystemManager* GetInstance();

    template<class T>
    T* RegisterSystem();

    template<class T>
    T* GetSystem();

    template<class T>
    void SetSystemSignature(Signature signature);

    void EntityDestroyed(EntityID entity);

    void EntitySignatureChanged(EntityID entity, Signature entitySignature);

    static ECSSystemManager* s_instance;

private:
    
    std::unordered_map<const char*, Signature> m_signatures;

    std::unordered_map<const char*, ECSSystembase*> m_systems;

};

template<class T>
T* ECSSystemManager::RegisterSystem() 
{
    const char* typeName = typeid(T).name();
    Assert(m_systems.find(typeName) == m_systems.end(), "ECSSystemManager - System already registered");
    auto system = new T();
    m_systems.insert({typeName, system});
    return system;
}

template<class T>
T* ECSSystemManager::GetSystem() 
{
    const char* typeName = typeid(T).name();
    Assert(m_systems.find(typeName) != m_systems.end(), "ECSSystemManager - System not registered");
    return dynamic_cast<T*>(m_systems[typeName]);
}

template<class T>
void ECSSystemManager::SetSystemSignature(Signature signature) 
{
    const char* typeName = typeid(T).name();
    Assert(m_systems.find(typeName) != m_systems.end(), "ECSSystemManager - System not registered");
    m_signatures.insert({typeName, signature});
}




} // namespace Gameplay
