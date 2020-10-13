#pragma once

#include <exception>
#include <string>

namespace Engine
{

class EngineException : public std::exception
{
private:
    std::string msg;
public:
    inline EngineException(std::string msg)
    {
        this->msg = msg;
    }

    const char* what() const throw()
    {
        return msg.c_str();
    }
};

class AssetLoadingException : public EngineException
{
public:
    inline AssetLoadingException(std::string msg) : EngineException(msg)
    {

    }
};

class FileNotFoundException : public EngineException
{
public:
    inline FileNotFoundException(std::string msg) : EngineException(msg)
    {

    }
};

} // namespace Engine
