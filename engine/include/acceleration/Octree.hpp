#pragma once

#include "core/GameObject.hpp"
#include "geometry/AxisAlignedBox.hpp"

#include <vector>

namespace Engine::Acceleration
{

class Octree
{
public:
    struct GOBB
    {
        Engine::Core::GameObject* GO;
        Engine::Geometry::AxisAlignedBox* BB;
    };

    struct Octan
    {
        Engine::Geometry::AxisAlignedBox* BoundingBox;
        std::vector<GOBB> GOBBS;
        Octan* octans[8];
        ~Octan();
    };
private:

    int _maxDepth = 0;

    

    Engine::Geometry::AxisAlignedBox* CalculateBoundingBox(std::vector<GOBB> volumes);

    Octan* CreateOctreeRecursive(std::vector<GOBB> input, Engine::Geometry::AxisAlignedBox* bb, int depth);

public:
    Octan* _root;
    std::vector<GOBB> data;
    Octree(std::vector<GOBB> input, int maxDepth);

    void Rebuild();

    ~Octree();
    
};

} // namespace Acceleration
