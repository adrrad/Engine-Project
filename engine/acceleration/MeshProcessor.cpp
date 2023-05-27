#include "acceleration/MeshProcessor.hpp"

#include "geometry/AxisAlignedBox.hpp"
#include "geometry/Point.hpp"
#include "utilities/MathUtils.hpp"
#include "utilities/Printing.hpp"

#include <queue>
#include <map>
#include <string>
#include <numeric>
#include <iostream>
#include <set>
#include <algorithm>

using namespace std;
using namespace glm;
namespace Acceleration
{

vector<Index> MeshProcessor::GetSimplifiedIndices(vector<Rendering::Vertex>& vertices, vector<Index>& indices, bool hasNormals)
{
    //FIXME: Currently works fine when performed once. Simyplifying more, some triangles disapear.
    //Compute triangles
    ElementCount numVertices = ElementCount(vertices.size());
    ElementCount numIndices = ElementCount(indices.size());
    ElementCount numTriangles = numIndices / 3;
    
    // All triangles
    vector<Triangle> triangles(numTriangles);
    // Mapping from vertex to triangles
    vector<vector<Index>> mapping(numVertices);
    // Map connecting a collapse and its cost. Also used to check whether a given collapse exists
    map<pair<int,int>, float> costMap;
    // Define a compare function for a priority queue used to pop cheapes collapses
    auto compare = [&](pair<int,int>& c1, pair<int,int>& c2)
    {
        return costMap[c1] < costMap[c2];
    };
    // Define the priority queue
    priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(compare)> pq(compare);
    // Calculate triangles
    for(Index triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        // Get triangle indices and vertices
        Index offset, i1, i2, i3;
        offset = triangleIndex * 3;
        i1 = indices[offset];
        i2 = indices[offset+1];
        i3 = indices[offset+2];
        Rendering::Vertex &v1 = vertices[i1], &v2 = vertices[i2], &v3 = vertices[i3];
        
        // Calculate triangle normal
        vec3 n;
        if(hasNormals) n = normalize(v1.Normal + v2.Normal + v3.Normal);
        else
        {
            vec3 e1 = normalize(v2.Position - v1.Position);
            vec3 e2 = normalize(v3.Position - v1.Position);
            n = normalize(cross(e1, e2));
        }
        // Calculate the plane parameter d
        float d = -(n.x * v1.Position.x + n.y + v1.Position.y + n.z + v1.Position.z);
        // Create triangle
        triangles[triangleIndex] = {i1, i2, i3, n, d};
        // Add mapping from vertex to triangle
        mapping[i1].push_back(triangleIndex);
        mapping[i2].push_back(triangleIndex);
        mapping[i3].push_back(triangleIndex);

        // i1 -> i2 & i1 -> i3 collapses
        if(costMap.contains({i1, i2})) costMap[{i1, i2}] += Cost(n, vertices[i2].Position, d);
        else costMap[{i1, i2}] = Cost(n, vertices[i2].Position, d);
        if(costMap.contains({i1, i3})) costMap[{i1, i3}] += Cost(n, vertices[i3].Position, d);
        else costMap[{i1, i3}] = Cost(n, vertices[i3].Position, d);
        // i2 -> i1 & i2 -> i3 collapses
        if(costMap.contains({i2, i1})) costMap[{i2, i1}] += Cost(n, vertices[i1].Position, d);
        else costMap[{i2, i1}] = Cost(n, vertices[i1].Position, d);
        if(costMap.contains({i2, i3})) costMap[{i2, i3}] += Cost(n, vertices[i3].Position, d);
        else costMap[{i2, i3}] = Cost(n, vertices[i3].Position, d);
        // i3 -> i1 & i3 -> i2 collapses
        if(costMap.contains({i3, i1})) costMap[{i3, i1}] += Cost(n, vertices[i1].Position, d);
        else costMap[{i3, i1}] = Cost(n, vertices[i1].Position, d);
        if(costMap.contains({i3, i2})) costMap[{i3, i2}] += Cost(n, vertices[i2].Position, d);
        else costMap[{i3, i2}] = Cost(n, vertices[i2].Position, d);
    }

    // Fill the priority queue
    for(auto& c : costMap) pq.push(c.first);

    vector<Index> vertexMapping(numVertices);
    vector<bool> vlocks(numVertices, false);
    iota(vertexMapping.begin(), vertexMapping.end(), 0);
    set<pair<int,int>> locked;
    // The new subset of indices
    vector<Index> indicesSubset;
    set<Index> ignored;
    ElementCount vert = 0, tri = 0;
    // Pop each element and simplify the mesh if the simplification is valid
    
    while(!pq.empty())
    {
        // Get the next proposed collapse
        auto col = pq.top();
        pq.pop();
        Index i = col.first;
        Index j = col.second;
        if(i == j) throw "Error, collapse onto itself";
        if(vlocks[i] || vlocks[j]) continue;
        bool valid = true;
        set<Index> candidates;

        for(Index triangleIndex : mapping[i])
        {
            Triangle& t = triangles[triangleIndex];
            if((t.i1 == i || t.i2 == i || t.i3 == i) && (t.i1 == j || t.i2 == j || t.i3 == j))
            {
                ignored.insert(triangleIndex);
                continue;
            }

            valid = IsTriangleValid(t, vertices, i, j);
            if(!valid) break;
            candidates.insert(triangleIndex);
        }
        if(!valid)
        {
            continue;
        }
        vertexMapping[i] = j;
        vlocks[i] = vlocks[j] = true;
    }

    for(Index triangleIndex = 0; triangleIndex < numTriangles; triangleIndex++)
    {
        if(ignored.contains(triangleIndex)) continue;
        Triangle& t = triangles[triangleIndex];
        indicesSubset.push_back(vertexMapping[t.i1]);
        indicesSubset.push_back(vertexMapping[t.i2]);
        indicesSubset.push_back(vertexMapping[t.i3]);
    }
    // cout << "Vertex " << vert << " Triangles " << tri << endl;
    // cout << "Old number of triangles: " << triangles.size() << endl;
    // cout << "New number of triangles: " << indicesSubset.size() / 3 << endl;
    // cout << "Old index count: " << numIndices << endl;
    // cout << "New index count: " << indicesSubset.size() << endl;
    return indicesSubset;
}


std::vector<std::vector<Index>> MeshProcessor::SubdivideTerrain(const std::vector<Rendering::Vertex>& vertices, const std::vector<Index>& indices, float minSize, int minVertices)
{
    // TODO: Can be optimized by using a helper recursive function that can accumulate indices and the related bounding box.
    // FIXME: Test with different amount of segment vertices. Can yield diconnected segments where the in-between triangles are missing
    // If any further division would yield fewer vertices than the specified amount, return the current set
    if(indices.size() / 4 < minVertices) return { indices };
    std::vector<std::vector<Index>> segments;
    // Get size
    vec3 min = vertices[0].Position, max = vertices[0].Position;
    vector<float> xs, zs;
    for(Index i : indices)
    {
        auto& v = vertices[i];
        min = Utilities::Min(v.Position, min);
        max = Utilities::Max(v.Position, max);
        // Store all x and z coordinates to get the median
        xs.push_back(v.Position.x);
        zs.push_back(v.Position.z);
    }
    vec3 dim = max - min;
    // If any further division would yield a size smaller than the specified amount, return the current set
    if(dim.x / 4 < minSize || dim.z / 4 < minSize) return { indices };
    // Sort the x and z coordinates
    sort(xs.begin(), xs.end());
    sort(zs.begin(), zs.end());
    // Calculate the center (median) (everything is handled in the XZ plane so the height is ignored)
    vec3 ctr = {xs[xs.size()/2], min.y, zs[zs.size()/2]};
    //Create 4 bounding boxes:
    Geometry::AxisAlignedBox b1 = Geometry::AxisAlignedBox(min, {ctr.x, max.y, ctr.z});
    Geometry::AxisAlignedBox b2 = Geometry::AxisAlignedBox({ctr.x, min.y, min.z}, {max.x, max.y, ctr.z});
    Geometry::AxisAlignedBox b3 = Geometry::AxisAlignedBox({min.x, min.y, ctr.z}, {ctr.x, max.y, max.z});
    Geometry::AxisAlignedBox b4 = Geometry::AxisAlignedBox({ctr.x, min.y, ctr.z}, max);
    vector<Index> is1, is2, is3, is4;
    //Assign triangles
    for(Index i = 0; i < indices.size(); i += 3)
    {
        Index i1 = indices[i], i2 = indices[i+1], i3 = indices[i+2];
        const vec3& v1 = vertices[i1].Position;
        const vec3& v2 = vertices[i2].Position;
        const vec3& v3 = vertices[i3].Position;
        
             if(b1.ContainsPoint(v1) && b1.ContainsPoint(v2) && b1.ContainsPoint(v3)) { is1.push_back(i1); is1.push_back(i2); is1.push_back(i3); }
        else if(b2.ContainsPoint(v1) && b2.ContainsPoint(v2) && b2.ContainsPoint(v3)) { is2.push_back(i1); is2.push_back(i2); is2.push_back(i3); }
        else if(b3.ContainsPoint(v1) && b3.ContainsPoint(v2) && b3.ContainsPoint(v3)) { is3.push_back(i1); is3.push_back(i2); is3.push_back(i3); }
        else if(b4.ContainsPoint(v1) && b4.ContainsPoint(v2) && b4.ContainsPoint(v3)) { is4.push_back(i1); is4.push_back(i2); is4.push_back(i3); }
    }

    vector<vector<Index>> result;
    for(auto& is : {is1, is2, is3, is4})
    {
        if(is.size() <= 0) continue;
        for(auto& recIS : SubdivideTerrain(vertices, is, minSize, minVertices)) result.push_back(recIS);
    }

    return result;
}


}