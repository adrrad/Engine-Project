#pragma once

#include "renderer/RenderingTypedefs.hpp"
#include "renderer/RenderingStructs.hpp"

#include <glm/glm.hpp>

namespace Engine::Acceleration
{

typedef uint32_t Index;
typedef uint32_t ElementCount;

class MeshSimplifier
{
private:
    struct Triangle
    {
        Index i1, i2, i3;
        glm::vec3 N;
        float d;
    };

    struct Collapse
    {
        Index i, j;
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

};


bool MeshSimplifier::IsTriangleValid(Triangle& t, std::vector<Rendering::Vertex>& vertices, Index oldVertex, Index newVertex)
{
    glm::vec3& normal = t.N;
    glm::vec3& v1 = t.i1 == oldVertex ? vertices[newVertex].Position : vertices[t.i1].Position;
    glm::vec3& v2 = t.i2 == oldVertex ? vertices[newVertex].Position : vertices[t.i2].Position;
    glm::vec3& v3 = t.i3 == oldVertex ? vertices[newVertex].Position : vertices[t.i3].Position;
    glm::vec3 e1 = glm::normalize(v2 - v1);
    glm::vec3 e2 = glm::normalize(v3 - v1);
    glm::vec3 newNormal = normalize(cross(e1, e2));
    return glm::dot(normal, newNormal) > 0.0f;
}

float MeshSimplifier::Cost(glm::vec3 n, glm::vec3 v, float d)
{
    float c = glm::dot(n, v) + d;
    return c*c;
}

} // namespace Engine::Acceleration
