#pragma once

#include "EngineTypedefs.hpp"

namespace Engine::Utilities
{
    
template<typename T>
class Array
{
private:
    T* _data;
public:
    const ElementCount Length;
    const SizeBytes Size;
    
    inline Array(ElementCount numElements) : Length(numElements), Size(sizeof(T)*numElements)
    {
        _data = (T*)malloc(Size);
    }

    inline T& operator[](Index i) { return _data[i]; };

    inline T* Data() { return _data; }
};

} // namespace Utilities
