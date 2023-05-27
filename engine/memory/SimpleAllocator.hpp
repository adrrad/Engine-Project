#pragma once

#include "Allocator.hpp"

#include "EngineTypedefs.hpp"

#include "Debug.hpp"

namespace Memory
{

class SimpleAllocator : public Allocator
{

struct Header
{
    u64 Allocated : 3;
    u64 Size : 61;

    inline bool IsAllocated()
    {
        return Allocated;
    }
};

public:

    SimpleAllocator(u64 size);
    
    SimpleAllocator(void* memory, u64 size);

    void* Allocate(u64 size) override;
    void Free(void* address) override;
private:

    void Initialise();

    void InitialiseBlockList();

    Header* FindFreeBlockOfSize(u64 minSize);

    inline Header* GetNextHeader(Header* current)
    {
        return (Header*)(((char*)current)+current->Size);
    }

    inline Header* AppendNewHeader(Header* prevHeader, u64 newHeaderSize)
    {
        //Get pointer to the new header location
        Header* newHeader = (Header*)(((char*)prevHeader)+prevHeader->Size);
        //Set size and allocation status
        newHeader->Allocated = 0;
        newHeader->Size = newHeaderSize;
        return newHeader;
    }

    inline void CoalesceBlocks(Header* h1, Header* h2)
    {
        Assert(!h1->IsAllocated() || !h2->IsAllocated(), "SimpleAllocator - Can't coalesce allocated blocks!");
        Assert(h1 < h2, "SimpleAllocator - Coalescion blocks out of order!");
        h1->Size += h2->Size;
    }

    void CheckCorruption();

    const u64 m_headerSize;
    u64 m_maxSize;
    char* m_memory;

    u64 m_allocatedMemory;

    Header* m_blockList;
};

};