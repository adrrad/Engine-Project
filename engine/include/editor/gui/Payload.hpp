#pragma once

#include <string>

namespace Engine::Editor
{
    
class IPayload
{
protected:
    virtual void Foo() {};
    static IPayload* CurrentPayload;
public:
    std::string name;

    template <class C>
    static void SetPayload(C* value);

    static inline IPayload* GetPayload()
    {
        if(CurrentPayload != nullptr)
        {
            auto val = CurrentPayload;
            CurrentPayload = nullptr;
            return val;
        }
        return nullptr;
    }
};

template <class C>
class Payload : public IPayload
{
public:
    C* Value;

    inline Payload(C* value) 
    { 
        Value = value; 
        name = typeid(C).name();
    }
};

template <class C>
void IPayload::SetPayload(C* value)
{
    if(CurrentPayload != nullptr) delete CurrentPayload;
    CurrentPayload = new Payload(value);
}

} // namespace Engine::Editor
