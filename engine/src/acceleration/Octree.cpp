#include "acceleration/Octree.hpp"

#include "utilities/MathUtils.hpp"
#include "utilities/Printing.hpp"
#include "components/MeshComponent.hpp"


#include <glm/glm.hpp>
#include <iostream>
#include <set>

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
    if(depth >= _maxDepth || input.size() <= 2)
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

Octree::Octree(vector<GOBB> input, int maxDepth)
{
    _maxDepth = maxDepth;
    data = input;
    _root = CreateOctreeRecursive(input, CalculateBoundingBox(input), 0);
}

Octree::~Octree()
{
    delete _root;
}


void Octree::Rebuild()
{
    delete _root;
    for(auto& a : data) a.BB = (Engine::Acceleration::AxisAlignedBox*)a.GO->GetComponent<Components::MeshComponent>()->GetBoundingVolume();
    _root = CreateOctreeRecursive(data, CalculateBoundingBox(data), 0);
}

std::set<Engine::Core::GameObject*> Octree::GetObjects(Engine::Geometry::Volume* bounds)
{
    std::set<Engine::Core::GameObject*> gobbs;
    GetObjectsRecursive(bounds, _root, gobbs);
    return gobbs;
}

} // namespace Acceleraction
