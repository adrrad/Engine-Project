#include "memory/MemoryManager.hpp"
#include "memory/Allocator.hpp"
#include "memory/SimpleAllocator.hpp"
#include "Debug.hpp"

namespace Memory
{

void MemoryManager::AllocateMemory(u64 size)
{
    m_baseMemoryAllocator = new SimpleAllocator(size);
}

Allocator* MemoryManager::GetAllocator(u64 size)
{
    Assert(m_baseMemoryAllocator != nullptr, "MemoryManager - No memory allocated but trying to get an allocator!");
    void* allocation = m_baseMemoryAllocator->Allocate(size);
    Assert(allocation != nullptr, "Allocator out of memory!");
    // TODO: place the allocator within the allocated memory
    return new SimpleAllocator(allocation, size);
}

void MemoryManager::Initialise()
{
    
}

MemoryManager::MemoryManager()
: m_baseMemoryAllocator(nullptr)
{
    
}

};