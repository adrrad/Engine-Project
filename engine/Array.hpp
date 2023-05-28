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
    const u64 Length;
    const u64 Size;
    
    inline Array(u64 Capacity) : Length(Capacity), Size(sizeof(T)*Capacity)
    {
        m_data = Capacity > 0 ? new T[Capacity] : nullptr;
    }

    inline Array(u64 Capacity, T* data) 
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

    inline void operator=(const Array<u64> &other)
    {
        memcpy(this, &other, sizeof(Array<T>));
        memcpy(m_data, other.m_data, other.Size);
    }

    inline T& operator()(u64 i) { return m_data[i]; };
    
    inline T& operator[](u64 i) { return m_data[i]; };

    inline T* Data() { return m_data; }
};

template<typename T>
class Array2D
{
private:
    T* m_data;
public:
    const u64 ColumnLength;
    const u64 RowLength;
    const u64 Size;
    
    inline Array2D(u64 columnLength, u64 rowLength) : ColumnLength(columnLength), RowLength(rowLength), Size(sizeof(T)*columnLength*rowLength)
    {
        m_data = (T*)malloc(Size);
    }

    inline T& operator()(u64 col, u64 row) { return m_data[col*ColumnLength+row]; };

    inline T* Data() { return m_data; }
};

