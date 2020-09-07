#include "acceleration/MeshSimplifier.hpp"

#include <queue>
#include <map>
#include <string>
#include <numeric>
#include <iostream>
#include <set>

using namespace std;
using namespace glm;
namespace Engine::Acceleration
{

vector<Index> MeshSimplifier::GetSimplifiedIndices(vector<Rendering::Vertex>& vertices, vector<Index>& indices, bool hasNormals)
{
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
    for(auto& c : costMap)
    {
        pq.push(c.first);
    }

    vector<Index> vertexMapping(numVertices);
    vector<bool> vlocks(numVertices, false);
    vector<bool> tlocks(numTriangles, false);
    set<Index> trianglesSubset;
    iota(vertexMapping.begin(), vertexMapping.end(), 0);
    Index numCollapses = 0, collapsedTriangles = 0;
    vector<Collapse> collapses;
    set<pair<int,int>> locked;
    // The new subset of indices
    vector<Index> indicesSubset;
    set<Index> ignored;
    ElementCount vert = 0, tri = 0;
    // Pop each element and simplify the mesh if the simplification is valid
    //TODO: Remove indices of removed triangles
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
    cout << "Vertex " << vert << " Triangles " << tri << endl;
    cout << "Old number of triangles: " << triangles.size() << endl;
    cout << "Number of collapsed triangles: " << collapsedTriangles << endl;
    cout << "New number of triangles: " << indicesSubset.size() / 3 << endl;
    cout << "Total collapses: " << numCollapses << endl;
    cout << "Old index count: " << numIndices << endl;
    cout << "New index count: " << indicesSubset.size() << endl;
    return indicesSubset;
}


}