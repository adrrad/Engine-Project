#pragma once

#include "rendering/RenderingTypedefs.hpp"
#include "rendering/RenderingStructs.hpp"

#include <glm/glm.hpp>

namespace Engine::Acceleration
{

typedef uint32_t Index;
typedef uint32_t ElementCount;

class MeshProcessor
{
private:
    struct Triangle
    {
        Index i1, i2, i3;
        glm::vec3 N;
        float d;
    };

    __forceinline static bool IsTriangleValid(Triangle& t, std::vector<Rendering::Vertex>& vertices, Index oldVertex, Index newVertex);

    /**
     * @brief Calculates the partial cost of moving a vertex o to new position v, given the triangle normal n.
     * 
     * @param n The triangle normal.
     * @param v The target position of the vertex.
     * @param d The original position of the vertex.
     * @return float 
     */
    __forceinline static float Cost(glm::vec3 n, glm::vec3 v, float d);

public:

    /**
     * @brief Performs a single pass of mesh simplification using a subset placement strategy, using the quadric error metric (QEM). For all triangles, at most one vertex per triangle is affected.
     * 
     * @param vertices The mesh vertices.
     * @param indices The mesh indices
     * @return std::vector<Index> A new set of indices.
     */
    static std::vector<Index> GetSimplifiedIndices(std::vector<Rendering::Vertex>& vertices, std::vector<Index>& indices, bool hasNormals);

    /**
     * @brief Divides a terrain into segments of at least minimum provided size or minimum amount of vertices.
     * 
     * @param vertices A vector of vertices.
     * @param indices A vector of indices.
     * @param minSize The size of minumum mesh segments.
     * @param minVertices The minimum amount of vertices per segments.
     * @return std::vector<std::vector<Index>> 
     */
    static std::vector<std::vector<Index>> SubdivideTerrain(const std::vector<Rendering::Vertex>& vertices, const std::vector<Index>& indices, float minSize, int minVertices);

};


bool MeshProcessor::IsTriangleValid(Triangle& t, std::vector<Rendering::Vertex>& vertices, Index oldVertex, Index newVertex)
{
    glm::vec3& normal = t.N;
    glm::vec3& v1 = t.i1 == oldVertex ? vertices[newVertex].Position : vertices[t.i1].Position;
    glm::vec3& v2 = t.i2 == oldVertex ? vertices[newVertex].Position : vertices[t.i2].Position;
    glm::vec3& v3 = t.i3 == oldVertex ? vertices[newVertex].Position : vertices[t.i3].Position;
    glm::vec3 e1 = glm::normalize(v2 - v1);
    glm::vec3 e2 = glm::normalize(v3 - v1);
    glm::vec3 newNormal = normalize(cross(e1, e2));
    float d = glm::dot(normal, newNormal);
    return d > 0.0f;
}

float MeshProcessor::Cost(glm::vec3 n, glm::vec3 v, float d)
{
    float c = glm::dot(n, v) + d;
    return c*c;
}

} // namespace Engine::Acceleration
