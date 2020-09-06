#include "acceleration/MeshSimplifier.hpp"

#include <queue>
#include <map>
#include <string>
#include <numeric>
#include <iostream>
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
    // All possible collapses
    vector<vector<Collapse>> collapses(numVertices);
    // The new indices to be returned. Initially a copy of the original indices
    vector<Index> newIndices = indices;
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
    }
    for(Index vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
    {
        ElementCount numConnections = mapping[vertexIndex].size() * 2;
        for(Index mappingIndex = 0; mappingIndex < mapping[vertexIndex].size(); mappingIndex++)
        {
            Index triangleIndex = mapping[vertexIndex][mappingIndex];
            Triangle& t = triangles[triangleIndex];
            string c1 = to_string(vertexIndex) + "-" + to_string(t.i1);
            string c2 = to_string(vertexIndex) + "-" + to_string(t.i1);
            if(vertexIndex != t.i1)
            {
                if(costMap.contains({vertexIndex, t.i1})) costMap[{vertexIndex, t.i1}] += Cost(t.N, vertices[t.i1].Position, t.d);
                else costMap[{vertexIndex, t.i1}] = Cost(t.N, vertices[t.i1].Position, t.d);
            }
            if(vertexIndex != t.i2)
            {
                if(costMap.contains({vertexIndex, t.i2})) costMap[{vertexIndex, t.i2}] += Cost(t.N, vertices[t.i2].Position, t.d);
                else costMap[{vertexIndex, t.i2}] = Cost(t.N, vertices[t.i2].Position, t.d);
            }
            if(vertexIndex != t.i3)
            {
                if(costMap.contains({vertexIndex, t.i3})) costMap[{vertexIndex, t.i3}] += Cost(t.N, vertices[t.i3].Position, t.d);
                else costMap[{vertexIndex, t.i3}] = Cost(t.N, vertices[t.i3].Position, t.d);
            }

        }
    }
    // Fill the priority queue
    for(auto& c : costMap)
    {
        pq.push(c.first);
    }

    vector<Index> vertexMapping(numVertices);
    vector<bool> locks(numVertices, false);
    iota(vertexMapping.begin(), vertexMapping.end(), 0);
    Index numCollapses = 0;
    // Pop each element and simplify the mesh if the simplification is valid
    //TODO: implement validity checking
    while(!pq.empty())
    {
        // Get the next proposed collapse
        auto col = pq.top();
        pq.pop();
        // If the source or target vertex was already moved, do not change anything.
        if(locks[col.first] || locks[col.second]) continue;
        // Check if new triangles would be valid
        Index i = col.first;
        Index j = col.second;
        bool isValid = true;
        for(Index triangleIndex : mapping[i])
        {
            Triangle& t = triangles[triangleIndex];
            // Do not include the collapsed triangle
            if((i == t.i1 || i == t.i2 || i == t.i3) && (j == t.i1 || j == t.i2 || j == t.i3)) continue;
            isValid = IsTriangleValid(t, vertices, i, j);
            if(!isValid) break;
        }
        if(!isValid) continue;
        // Replace the index of that vertex
        vertexMapping[col.first] = col.second; 
        locks[col.first] = true;
        numCollapses++;
    }
    // Re-map the indices
    for(Index i = 0; i < numIndices; i++)
    {
        newIndices[i] = vertexMapping[newIndices[i]];
    }
    cout << "Nuber of triangles: " << triangles.size() << endl;
    cout << "Total collapses: " << numCollapses << endl;
    return newIndices;

    // 
    // float d = -(n.x * v1.Position.x + n.y + v1.Position.y + n.z + v1.Position.z);
    // // Generate the collapses, 2 per vertex, 6 in total
    // // v1 -> v2 and v1 -> v3
    // collapses[i1].push_back({i1, i2, Cost(n, v2.Position, d)});
    // collapses[i1].push_back({i1, i3, Cost(n, v3.Position, d)});
    // // v2 -> v1 and v2 -> v3
    // collapses[i2].push_back({i2, i2, Cost(n, v2.Position, d)});
    // collapses[i2].push_back({i2, i2, Cost(n, v3.Position, d)});
    // // v1 -> v2 and v1 -> v3
    // collapses[i3].push_back({i3, i2, Cost(n, v2.Position, d)});
    // collapses[i3].push_back({i3, i2, Cost(n, v3.Position, d)});
}


}