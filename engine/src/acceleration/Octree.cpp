#include "acceleration/Octree.hpp"

#include "core/GameObject.hpp"

#include "utilities/MathUtils.hpp"
#include "utilities/Printing.hpp"
#include "components/MeshComponent.hpp"


#include <glm/glm.hpp>
#include <iostream>
#include <set>
#include <queue>

using namespace glm;
using namespace std;

namespace Engine::Acceleration
{
typedef Octree::GOBB GOBB;
typedef Engine::Geometry::AxisAlignedBox AxisAlignedBox;

Octree::Octan::~Octan()
{
    delete BoundingBox;
    GOBBS.clear();
    for(int i = 0; i < 8; i++)
    {
        delete octans[i];
    }
}

AxisAlignedBox* Octree::CalculateBoundingBox(vector<GOBB> volumes)
{
    if(volumes.size() == 0) throw exception("No volumes to calculate BB on!");
    vec3 min = volumes[0].BB->Min;
    vec3 max = volumes[0].BB->Max;
    for(auto& gobb : volumes)
    {
        min = Utilities::Min(min, gobb.BB->Min);
        max = Utilities::Max(max, gobb.BB->Max);
    }
    return new AxisAlignedBox(min, max);
}

Octree::Octan* Octree::CreateOctreeRecursive(vector<GOBB> input, Engine::Geometry::AxisAlignedBox* bb, int depth)
{
    Octan* octan = new Octan();
    octan->BoundingBox = bb;
    if(depth >= m_maxDepth || input.size() <= 2)
    {
        octan->GOBBS = input;
        return octan;
    }
    vector<GOBB> octs[8];
    AxisAlignedBox* bbs[8];
    vec3 center = bb->Min + (bb->Max - bb->Min) * 0.5f;
    vec3 min = bb->Min;
    vec3 max = bb->Max;
    bbs[0] = new AxisAlignedBox(center, max);
    bbs[1] = new AxisAlignedBox(vec3(center.x, center.y, min.z), vec3(max.x, max.y, center.z));
    bbs[2] = new AxisAlignedBox(vec3(center.x, min.y, center.z), vec3(max.x, center.y, max.z));
    bbs[3] = new AxisAlignedBox(vec3(center.x, min.y, min.z), vec3(max.x, center.y, center.z));
    bbs[4] = new AxisAlignedBox(vec3(min.x, center.y, center.z), vec3(center.x, max.y, max.z));
    bbs[5] = new AxisAlignedBox(vec3(min.x, center.y, min.z), vec3(center.x, max.y, center.z));
    bbs[6] = new AxisAlignedBox(vec3(min.x, min.y, center.z), vec3(center.x, center.y, max.z));
    bbs[7] = new AxisAlignedBox(min, center);
    // Utilities::Print("Center");
    // Utilities::Print(center);
    // Utilities::Print("Min");
    // Utilities::Print(bb->Min);
    // Utilities::Print("Max");
    // Utilities::Print(bb->Max);
    for(auto& gobb : input)
    {
        min = gobb.BB->Min;
        max = gobb.BB->Max;
        
        //In front of YZ plane
        if(max.x > center.x)
        {
            //Above XZ plane
            if(max.y > center.y)
            {
                //In front of XY plane
                if(max.z > center.z)
                {
                    octs[0].push_back(gobb);
                }
                //Behind XY plane
                if(min.z <= center.z)
                {
                    octs[1].push_back(gobb);
                }
            }
            //Below XZ plane
            if(min.y <= center.y)
            {
                //In front of XY plane
                if(max.z > center.z)
                {
                    octs[2].push_back(gobb);
                }
                //Behind XY plane
                if(min.z <= center.z)
                {
                    octs[3].push_back(gobb);                    
                }
            }
        }
        //Behind YZ plane
        if(min.x < center.x)
        {
            //Above XZ plane
            if(max.y > center.y)
            {
                //In front of XY plane
                if(max.z > center.z)
                {
                    octs[4].push_back(gobb);
                }
                //Behind XY plane
                if(min.z <= center.z)
                {
                    octs[5].push_back(gobb);                    
                }
            }
            //Below XZ plane
            if(min.y <= center.y)
            {
                //In front of XY plane
                if(max.z > center.z)
                {
                    octs[6].push_back(gobb);
                }
                //Behind XY plane
                if(min.z <= center.z)
                {
                    octs[7].push_back(gobb);                    
                }
            }
        }
    }

    for(int index = 0; index < 8; index++)
    {
        if(octs[index].size() > 0)
        {
            octan->octans[index] = CreateOctreeRecursive(octs[index], bbs[index], depth+1);
        }
        else
        {
            delete bbs[index];
        }
    }
    return octan;
}

void Octree::GetObjectsRecursive(Engine::Geometry::Volume* bounds, Octree::Octan* octan, std::set<Engine::Core::GameObject*>& acc)
{
    if(!bounds->IntersectsAxisAlignedBox(octan->BoundingBox)) return;
    if(octan->GOBBS.size() > 0)
    {
        for(auto& gobb : octan->GOBBS)
        {
            if(bounds->IntersectsAxisAlignedBox(gobb.BB))
            {
                acc.insert(gobb.GO);
            }

        } 
    }
    for(int i = 0; i < 8; i++)
    {
        if(octan->octans[i] != nullptr)
        {
            GetObjectsRecursive(bounds, octan->octans[i], acc);
        }
    }
}

void Octree::RecordRenderpassRecursive(Engine::Geometry::Volume* bounds, Octan* octan, Rendering::Renderpass::RenderpassBuilder &rpb, std::set<Engine::Core::GameObject*>& acc)
{
    if(!bounds->IntersectsAxisAlignedBox(octan->BoundingBox)) return;
    if(octan->GOBBS.size() > 0)
    {
        for(auto& gobb : octan->GOBBS)
        {
            if(!gobb.GO->Enabled()) continue;
            if(bounds->IntersectsAxisAlignedBox(gobb.BB) && !acc.contains(gobb.GO))
            {
                rpb.DrawMesh(gobb.GO->GetComponent<Components::MeshComponent>());
                acc.insert(gobb.GO);
            }

        } 
    }
    for(int i = 0; i < 8; i++)
    {
        if(octan->octans[i] != nullptr)
        {
            RecordRenderpassRecursive(bounds, octan->octans[i], rpb, acc);
        }
    }
}

Octree::Octree(vector<GOBB> input, int maxDepth)
{
    m_maxDepth = maxDepth;
    m_data = input;
    m_root = CreateOctreeRecursive(input, CalculateBoundingBox(input), 0);
}

Octree::Octree(std::vector<Core::GameObject*> input, int maxDepth)
{
    for(auto go : input)
    {
        auto mc = go->GetComponent<Components::MeshComponent>();
        if(mc) m_data.push_back({go, (Geometry::AxisAlignedBox*)mc->GetBoundingVolume()});
    }
    m_maxDepth = maxDepth;
    if(m_data.size() == 0) 
    {
        m_root = new Octan();
        m_root->BoundingBox = new AxisAlignedBox({0,0,0}, {0,0,0});
    }
    else
    {
        m_root = CreateOctreeRecursive(m_data, CalculateBoundingBox(m_data), 0);
    }
}

Octree::Octree(std::vector<Components::MeshComponent*> input, int maxDepth)
{
    for(auto mc : input)
    {
        if(mc) m_data.push_back({mc->GetGameObject(), (Geometry::AxisAlignedBox*)mc->GetBoundingVolume()});
    }
    m_maxDepth = maxDepth;
    m_root = CreateOctreeRecursive(m_data, CalculateBoundingBox(m_data), 0);
}

Octree::~Octree()
{
    delete m_root;
}


void Octree::Rebuild()
{
    delete m_root;
    for(auto& a : m_data) a.BB = (Engine::Acceleration::AxisAlignedBox*)a.GO->GetComponent<Components::MeshComponent>()->GetBoundingVolume();
    m_root = CreateOctreeRecursive(m_data, CalculateBoundingBox(m_data), 0);
}

std::set<Engine::Core::GameObject*> Octree::GetObjects(Engine::Geometry::Volume* bounds)
{
    std::set<Engine::Core::GameObject*> gobbs;
    GetObjectsRecursive(bounds, m_root, gobbs);
    return gobbs;
}


void Octree::RecordRenderpass(Engine::Geometry::Volume* bounds, Rendering::Renderpass::RenderpassBuilder &rpb)
{
    std::set<Engine::Core::GameObject*> gobbs;
    RecordRenderpassRecursive(bounds, m_root, rpb, gobbs);
}

void Octree::ForEach(Engine::Geometry::Volume* bounds, std::function<void(Components::MeshComponent*)> lambda)
{
    std::queue<Octan*> octans;
    std::set<Core::GameObject*> visited;
    octans.push(m_root);
    while(!octans.empty())
    {
        Octan* current = octans.front();
        octans.pop();
        for(auto go : current->GOBBS)
        {
            if(bounds->IntersectsAxisAlignedBox(go.BB))
            {
                if(visited.contains(go.GO)) continue;
                visited.insert(go.GO);
                if(auto mc = go.GO->GetComponent<Components::MeshComponent>())
                {
                    lambda(mc);
                }
            }
        }
        for(int i = 0; i < 8; i++)
        {
            if(current->octans[i] != nullptr && bounds->IntersectsAxisAlignedBox(current->octans[i]->BoundingBox)) 
                octans.push(current->octans[i]);
        }
    }
}

} // namespace Acceleraction
