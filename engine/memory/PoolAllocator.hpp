#pragma once

#include "Array.hpp"
#include "EngineTypedefs.hpp"
#include "Debug.hpp"
#include <queue>

namespace Memory
{

template<class T>
class PoolAllocator
{
public:
    
    PoolAllocator(u64 capacity);

    T* Allocate();

    void Free(T* object);

private:

    void Initialise();

    Array<T> m_pool;
    std::queue<u64> m_freeIndices;
};

template<class T>
PoolAllocator<T>::PoolAllocator(u64 capacity) 
: m_pool(capacity)
{
    Initialise();
}


template<class T>
T* PoolAllocator<T>::Allocate() 
{
    if(m_freeIndices.size() == 0)
    {
        AssertFail("PoolAllocator - Max capacity reached, can't allocate more!");
        return nullptr;
    }
    u64 index = m_freeIndices.front();
    m_freeIndices.pop();
    return &m_pool[index];
}

template<class T>
void PoolAllocator<T>::Free(T* object) 
{
    // Convert pointer address to uint
    u64 address = reinterpret_cast<u64>(object);
    u64 startAddress = reinterpret_cast<u64>(m_pool[0]);
    
    bool withinRange = (address > startAddress) && (address < (startAddress + m_pool.Size));
    if(!withinRange)
    {
        AssertFail("PoolAllocator - Can't free an object that is not allocated from this pool!");
        return;
    }
    object->~T();
    u64 index = (address - startAddress) / sizeof(T);
    m_freeIndices.push(index);
}

template<class T>
void PoolAllocator<T>::Initialise() 
{
    for(u64 index = 0; index < m_pool.Length; index++)
    {
        m_freeIndices.push(index);
    }
}


} // namespace Memory
