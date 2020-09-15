#pragma once
#include "core/EngineSettings.hpp"

namespace Engine::Core
{

class EngineSubsystem
{

public:
    virtual void Update(float deltaTime) = 0;

};

} // namespace Engine::Core