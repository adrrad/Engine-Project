#pragma once

#include "EngineComponent.hpp"

#include "EngineTypedefs.hpp"

#include <unordered_map>

namespace Memory
{

class Allocator;

class MemoryManager : public EngineSubsystem<MemoryManager>
{
friend class EngineSubsystem<MemoryManager>;
public:

    void AllocateMemory(u64 size);
    
    Allocator* GetAllocator(u64 size);

    template<class T>
    Allocator* CreateAllocator(u64 size);

    template<class T>
    Allocator* GetAllocatorForSubsystem();

    void Initialise() override;

private:
    MemoryManager();

    std::unordered_map<const char*, Allocator*> m_sybsystemAllocators;

    Allocator* m_baseMemoryAllocator;

};

template<class T>
Allocator* MemoryManager::CreateAllocator(u64 size)
{
    const char* typeName = typeid(T).name();
    if(m_sybsystemAllocators.contains(typeName))
    {
        AssertFail("Memory Manager - Allocator for subsystem already created!");
        return nullptr;
    }
    Allocator* allocator = GetAllocator(size);
    m_sybsystemAllocators.insert({typeName, allocator});
    return allocator;
}

template<class T>
Allocator* MemoryManager::GetAllocatorForSubsystem()
{
    const char* typeName = typeid(T).name();
    if(m_sybsystemAllocators.contains(typeName))
    {
        return m_sybsystemAllocators[typeName];
    }
    return nullptr;
}

};