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
    
    inline std::vector<std::string> Split(const std::string str, const std::string& delimiter)
    {
        std::vector<std::string> out;
        std::string text = str;
        int pos = 0;
        while((pos = int(text.find(delimiter))) != std::string::npos)
        {
            out.push_back(text.substr(0, pos));
            text.erase(0, pos + delimiter.length());
        }
        out.push_back(text);
        return out;
    }

    inline std::string SplitOnFirst(const std::string str, const std::string& delimiter, bool returnFront = false)
    {
        std::string text = str;
        int pos = 0;
        if((pos = int(text.find(delimiter))) != std::string::npos)
        {
            if(returnFront) return text.erase(pos, text.size()-pos);
            else return text.erase(0, pos + delimiter.length());
            
        } 
        return text;
    }

    inline int IndexOfNext(const std::string str, const std::string& substring)
    {
        int pos = 0;
        if((pos = int(str.find(substring))) != std::string::npos)
            return pos;
        return 0;
    }

    inline std::string NextDouble(const std::string str, int offset = 0)
    {
        std::string text = str.substr(offset);
        std::string::size_type sz;
        std::stod(text, &sz);
        return text.substr(0, sz);
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
        return out + text;
    }

    inline bool Contains(const std::string& str, std::string substring)
    {;
        return str.find(substring) != std::string::npos;
    }

    inline bool ContainsAtIndex(const std::string& str, std::string substring, int index)
    {
        return str.substr(index, substring.size()).find(substring) != std::string::npos;
    }

    inline bool Match(const std::string& str, std::vector<std::string> cases)
    {
        for(auto& s : cases) if(str == s) return true;
        return false;
    }

    inline std::string Repeat(const std::string substring, int n)
    {
        std::string out = "";
        for(int i = 0; i < n; i++) out += substring;
        return out;
    }

    inline std::string ToLower(std::string str)
    {
        std::string newstr = str;
        for(char& c : newstr)
        {
            c = std::tolower(c);
        }
        return newstr;
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