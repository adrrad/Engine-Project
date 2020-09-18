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

    inline char* GetCharPtr(std::string str)
    {
        char* chars = new char[str.size() + 1];
        std::copy(str.begin(), str.end(), chars);
        chars[str.size()] = '\0';
        return chars;
    }
    
    inline std::vector<std::string> Split(const std::string& str, const std::string& delimiter)
    {
        std::vector<std::string> out;
        std::string text = str;
        int pos = 0;
        while((pos = int(text.find(delimiter))) != std::string::npos)
        {
            out.push_back(text.substr(0, pos));
            text.erase(0, pos + delimiter.length());
        }
        return out;
    }

    inline std::string SplitOnFirst(const std::string& str, const std::string& delimiter)
    {
        std::string text = str;
        int pos = 0;
        if((pos = int(text.find(delimiter))) != std::string::npos) return text.erase(0, pos + delimiter.length());
        return str;

    }

    inline std::string Replace(const std::string& str, const std::string& oldSubstring, const std::string& newSubstring)
    {
        std::string out;
        std::string text = str;
        int pos = 0;
        while((pos = int(text.find(oldSubstring))) != std::string::npos)
        {
            out += text.substr(0, pos) + newSubstring;
            text.erase(0, pos + oldSubstring.length());
        }
        return out;
    }

    inline bool Contains(const std::string& str, std::string substring)
    {;
        return str.find(substring) != std::string::npos;
    }

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