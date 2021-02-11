#pragma once

#include "geometry/AxisAlignedBox.hpp"
#include "rendering/Renderpass.hpp"

#include <functional>
#include <vector>
#include <set>

namespace Engine::Core
{
    class GameObject;
}

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

    int m_maxDepth = 0;

    Octan* m_root;
    
    std::vector<GOBB> m_data;

    Engine::Geometry::AxisAlignedBox* CalculateBoundingBox(std::vector<GOBB> volumes);

    Octan* CreateOctreeRecursive(std::vector<GOBB> input, Engine::Geometry::AxisAlignedBox* bb, int depth);

    void GetObjectsRecursive(Engine::Geometry::Volume* bounds, Octan* octan, std::set<Engine::Core::GameObject*>& acc);
    
    void RecordRenderpassRecursive(Engine::Geometry::Volume* bounds, Octan* octan, Rendering::Renderpass::RenderpassBuilder &rpb, std::set<Engine::Core::GameObject*>& acc);

public:
    Octree(std::vector<GOBB> input, int maxDepth);

    Octree(std::vector<Core::GameObject*> input, int maxDepth);

    Octree(std::vector<Components::MeshComponent*> input, int maxDepth);

    ~Octree();

    void Rebuild();

    std::set<Engine::Core::GameObject*> GetObjects(Engine::Geometry::Volume* bounds);

    void RecordRenderpass(Engine::Geometry::Volume* bounds, Rendering::Renderpass::RenderpassBuilder &rpb);

    /**
     * @brief Iterates through each MeshComponent within the given bounds and applies the lambda function.
     * 
     * @param bounds 
     * @param lambda 
     */
    void Apply(Engine::Geometry::Volume* bounds, std::function<void(Components::MeshComponent*)> lambda);

};

} // namespace Acceleration
