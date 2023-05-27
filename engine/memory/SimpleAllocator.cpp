#include "memory/SimpleAllocator.hpp"

#include "utilities/MathUtils.hpp"

#include "Debug.hpp"
#include "imgui.h"


#include <stdio.h>

namespace Memory
{

SimpleAllocator::SimpleAllocator(u64 size) : SimpleAllocator(malloc(size), size)
{
    
}

SimpleAllocator::SimpleAllocator(void* memory, u64 size)
: m_maxSize(size)
, m_memory((char*)memory)
, m_headerSize(sizeof(Header))
, m_blockList(nullptr)
, m_allocatedMemory(0)
{
    Initialise();
}

void* SimpleAllocator::Allocate(u64 size)
{
    u64 totalSize = size + m_headerSize;
    totalSize = Utilities::RoundUp(totalSize, 8);
    Header* header = FindFreeBlockOfSize(totalSize);

    if(header == nullptr)
    {
        return nullptr;
    }

    u64 excess = header->Size - totalSize;
    header->Size = totalSize;
    header->Allocated = true;
    AppendNewHeader(header, excess);
    void* allocation = ((char*)header) + m_headerSize;
    m_allocatedMemory += header->Size;
    return allocation;
}

void SimpleAllocator::Free(void* address)
{
    //TODO: Check if address is within bounds
    Header* current = m_blockList;
    while(current->Size > 0)
    {
        if(!current->IsAllocated())
        {
            current = GetNextHeader(current);
            continue;
        }
        char* currentAddress = (char*)current;
        void* allocationAddress = currentAddress + m_headerSize;
        if(allocationAddress == address)
        {
            current->Allocated = 0;
            m_allocatedMemory -= current->Size;
            return;
        }
        current = GetNextHeader(current);
    }
}

void SimpleAllocator::Initialise()
{
    InitialiseBlockList();
}

void SimpleAllocator::InitialiseBlockList()
{
    if(m_blockList != nullptr)
    {
        return;
    }

    Assert(m_memory != nullptr, "SimpleAllocator - nullptr memory pointer provided!");
    // Initial block
    m_blockList = (Header*)m_memory;
    m_blockList->Allocated = 0;
    m_blockList->Size = m_maxSize - m_headerSize;

    // List tail
    Header end;
    end.Allocated = 1;
    end.Size = 0;
    *((Header*)(m_memory + m_blockList->Size)) = end;

}

SimpleAllocator::Header* SimpleAllocator::FindFreeBlockOfSize(u64 minSize)
{
    Header* current = m_blockList;

    while(current->Size > 0)
    {
        if(current->IsAllocated())
        {
            current = GetNextHeader(current);
            continue;
        }

        if(current->Size >= minSize + m_headerSize)
        {
            return current;
        }

        // Check next block
        Header* next = GetNextHeader(current);
        if(!next->IsAllocated())
        {
            // If next block is free, coalesce with the current and try it again
            CoalesceBlocks(current, next);
        }
        else
        {
            // Else just continue the search
            current = next;
        }
    }
    return nullptr;
}

void SimpleAllocator::CheckCorruption()
{
    u64 size = m_headerSize; // Include 8 bytes tail header 
    Header* current = m_blockList;
    while(current->Size > 0)
    {
        size += current->Size;
        current = GetNextHeader(current);
    }
    char msg[150];
    sprintf_s(msg, "Memory corruption! Allocator max size should be %llu but is %llu.", m_maxSize, size);
    Assert(size == m_maxSize, msg);
}

};