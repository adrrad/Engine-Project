#pragma once

#include <chrono>


namespace Engine::Utilities
{

class Clock
{
private:
    std::chrono::steady_clock::time_point absoluteStartPoint;
    std::chrono::steady_clock::time_point startPoint;

public:

    Clock()
    {
        startPoint = std::chrono::high_resolution_clock::now();
    }

    __forceinline void Start()
    {
        startPoint = std::chrono::high_resolution_clock::now();
    }

    __forceinline float Stop()
    {
        std::chrono::duration<float> diff = std::chrono::high_resolution_clock::now() - startPoint;
        return diff.count();
    }

};

} // namespace Engine::Core
