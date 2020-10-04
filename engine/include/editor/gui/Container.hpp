#pragma once

#include <stdint.h>

namespace Engine::Editor
{ 

class Container
{
public: 
    uint32_t X = 0;
    uint32_t Y = 0;
    uint32_t Width = 0;
    uint32_t Height = 0;
    uint32_t MinWidth = 0;
    uint32_t MaxWidth = 0;
    uint32_t MinHeight = 0;
    uint32_t MaxHeight = 0;
};

} // namespace Engine::Editor
