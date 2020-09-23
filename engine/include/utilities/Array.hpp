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
    
    inline Array(ElementCount Capacity) : Length(Capacity), Size(sizeof(T)*Capacity)
    {
        _data = (T*)malloc(Size);
    }

    inline Array(ElementCount Capacity, T* data) : Length(Capacity), Size(sizeof(T)*Capacity)
    {
        _data = data;
    }

    inline T& operator()(Index i) { return _data[i]; };

    inline T* Data() { return _data; }
};

template<typename T>
class Array2D
{
private:
    T* _data;
public:
    const ElementCount ColumnLength;
    const ElementCount RowLength;
    const SizeBytes Size;
    
    inline Array2D(ElementCount columnLength, ElementCount rowLength) : ColumnLength(columnLength), RowLength(rowLength), Size(sizeof(T)*columnLength*rowLength)
    {
        _data = (T*)malloc(Size);
    }

    inline T& operator()(Index col, Index row) { return _data[col*ColumnLength+row]; };

    inline T* Data() { return _data; }
};

} // namespace Engine::Utilities
