#pragma once

namespace Engine::Editor
{

class SelectableItem
{
    virtual void Foo() {};
};

template <class C>
class SelectedItem : public SelectableItem
{
public:
    C* Value;

    inline SelectedItem(C* val) : Value(val) {}
};

    
} // namespace Engine::Editor
