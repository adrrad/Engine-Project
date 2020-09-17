#pragma once

#include <vector>
#include <algorithm>
#include <string>

namespace Engine::Utilities
{   
    class StringConvertable
    {
        public:
            virtual std::string ToString() = 0;
    };

    inline std::string ToString (int val)
    {
        return std::to_string(val);
    }

    inline std::string ToString (long val)
    {
        return std::to_string(val);
    }

    inline std::string ToString (long long val)
    {
        return std::to_string(val);
    }

    inline std::string ToString (unsigned val)
    {
        return std::to_string(val);
    }

    inline std::string ToString (unsigned long val)
    {
        return std::to_string(val);
    }

    inline std::string ToString (unsigned long long val)
    {
        return std::to_string(val);
    }

    inline std::string ToString (float val)
    {
        return std::to_string(val);
    }

    inline std::string ToString (double val)
    {
        return std::to_string(val);
    }

    inline std::string ToString (long double val)
    {
        return std::to_string(val);
    }

    inline std::string ToString (StringConvertable* val)
    {
        return val->ToString();
    } 
}