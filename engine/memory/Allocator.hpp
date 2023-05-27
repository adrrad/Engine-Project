#pragma once

#include "EngineTypedefs.hpp"
#include <iostream>
namespace Memory
{

class Allocator
{
public:
    virtual void* Allocate(u64 size) = 0;
    virtual void Free(void* address) = 0;

    template<typename T>
    T* Allocate()
    {
        return (T*)Allocate(sizeof(T));
    }

    template <class T, class... Types>
    T* New(Types&&... Args) 
    {
        return new (Allocate<T>()) T(Args...);
    }

};

};