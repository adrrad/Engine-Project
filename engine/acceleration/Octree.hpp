#pragma once

#include "geometry/AxisAlignedBox.hpp"
#include "rendering/Renderpass.hpp"

#include <functional>
#include <vector>
#include <set>

namespace Gameplay
{
    class GameObject;
}

namespace Acceleration
{

class Octree
{
public:
    struct GOBB
    {
        Gameplay::GameObject* GO;
        Geometry::AxisAlignedBox* BB;
    };

    struct Octan
    {
        Geometry::AxisAlignedBox* BoundingBox;
        std::vector<GOBB> GOBBS;
        Octan* octans[8];
        ~Octan();
    };
private:

    int m_maxDepth = 0;

    Octan* m_root;
    
    std::vector<GOBB> m_data;

    Geometry::AxisAlignedBox* CalculateBoundingBox(std::vector<GOBB> volumes);

    Octan* CreateOctreeRecursive(std::vector<GOBB> input, Geometry::AxisAlignedBox* bb, int depth);

    void GetObjectsRecursive(Geometry::Volume* bounds, Octan* octan, std::set<Gameplay::GameObject*>& acc);
    
    void RecordRenderpassRecursive(Geometry::Volume* bounds, Octan* octan, Rendering::Renderpass::RenderpassBuilder &rpb, std::set<Gameplay::GameObject*>& acc);

public:
    Octree(std::vector<GOBB> input, int maxDepth);

    Octree(std::vector<Gameplay::GameObject*> input, int maxDepth);

    Octree(std::vector<Gameplay::MeshComponent*> input, int maxDepth);

    ~Octree();

    void Rebuild();

    std::set<Gameplay::GameObject*> GetObjects(Geometry::Volume* bounds);

    void RecordRenderpass(Geometry::Volume* bounds, Rendering::Renderpass::RenderpassBuilder &rpb);

    /**
     * @brief Iterates through each MeshComponent within the given bounds and applies the lambda function.
     * 
     * @param bounds 
     * @param lambda 
     */
    void ForEach(Geometry::Volume* bounds, std::function<void(Gameplay::MeshComponent*)> lambda);

};

} // namespace Acceleration
