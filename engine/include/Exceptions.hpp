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

} // namespace Engine
