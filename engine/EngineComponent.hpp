#pragma once

#include "EngineTypedefs.hpp"
#include "memory/Allocator.hpp"
#include "Debug.hpp"

class EngineComponent
{
protected:
    Memory::Allocator* m_allocator;

    template <class T, class... Types>
    T* New(Types&&... Args) 
    {
        Assert(m_allocator != nullptr, "EngineComponent - No allocator set!");
        Assert(!std::is_fundamental<T>::value, "EngineComponent - New should only be used for class types, use Array class for primitive types!");
        return new (m_allocator->Allocate<T>()) T(Args...);
    }

    template <class T, class... Types>
    void Delete(T* obj)
    {
        Assert(m_allocator != nullptr, "EngineComponent - No allocator set!");
        Assert(!std::is_fundamental<T>::value, "EngineComponent - Delete should only be used for class objects!");
        obj->~T();
        m_allocator->Free(obj);
    }

public:
    EngineComponent() : m_allocator(nullptr) {};

    inline void SetAllocator(Memory::Allocator* allocator)
    {
        m_allocator = allocator;
    }

};

template<typename T>
class EngineSubsystem : public EngineComponent
{

private:
    static T* s_instance;

public:
    static T* GetInstance()
    {
        if(s_instance == nullptr)
        {
            s_instance = new T();
        }
        return s_instance;
    }

    virtual void Initialise() = 0;

    static Memory::Allocator* GetSubsystemAllocator()
    {
        Assert(s_instance->m_allocator != nullptr, "EngineSubsystem - No allocator set!");
        return s_instance->m_allocator;
    }

    virtual void Update(float deltaTime) {};
};

template<typename T>
T* EngineSubsystem<T>::s_instance = nullptr;

template<class SubsystemType>
class SubsystemComponent : public EngineComponent
{
public:

    SubsystemComponent();

};
template<class SubsystemType>
SubsystemComponent<SubsystemType>::SubsystemComponent() 
{
    SetAllocator(EngineSubsystem<SubsystemType>::GetSubsystemAllocator());
}