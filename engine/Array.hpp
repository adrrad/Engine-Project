#pragma once

#include "EngineTypedefs.hpp"
    
#include <vector>

template<class T>
using Vector = std::vector<T>;

template<typename T>
class Array
{
private:
    T* m_data;
public:
    const ElementCount Length;
    const SizeBytes Size;
    
    inline Array(ElementCount Capacity) : Length(Capacity), Size(sizeof(T)*Capacity)
    {
        m_data = Capacity > 0 ? new T[Capacity] : nullptr;
    }

    inline Array(ElementCount Capacity, T* data) 
        : Array(Capacity)
    {
        memcpy(m_data, data, Size);
    }

    inline Array(const Array<T>& other) 
        : Array(other.Length)
    {
        memcpy(m_data, other.m_data, Size);
    }

    ~Array()
    {
        if(m_data != nullptr)
        {
            delete[] m_data;
        }
    }

    inline void operator=(const Array<Index> &other)
    {
        memcpy(this, &other, sizeof(Array<T>));
        memcpy(m_data, other.m_data, other.Size);
    }

    inline T& operator()(Index i) { return m_data[i]; };
    
    inline T& operator[](Index i) { return m_data[i]; };

    inline T* Data() { return m_data; }
};

template<typename T>
class Array2D
{
private:
    T* m_data;
public:
    const ElementCount ColumnLength;
    const ElementCount RowLength;
    const SizeBytes Size;
    
    inline Array2D(ElementCount columnLength, ElementCount rowLength) : ColumnLength(columnLength), RowLength(rowLength), Size(sizeof(T)*columnLength*rowLength)
    {
        m_data = (T*)malloc(Size);
    }

    inline T& operator()(Index col, Index row) { return m_data[col*ColumnLength+row]; };

    inline T* Data() { return m_data; }
};

