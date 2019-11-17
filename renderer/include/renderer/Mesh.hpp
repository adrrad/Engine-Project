#pragma once
#include "renderer/Vertex.hpp"

#include <cstdint>
#include <vector>

namespace Rendering
{

class Mesh
{

private:
    uint32_t _vbo = 0;
    uint32_t _ebo = 0;
    uint32_t _vao = 0;
    uint32_t _shader = 0;
    uint32_t _vertexCount = 0;
    uint32_t _indexCount = 0;
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, uint32_t shader);

    uint32_t GetVAO();
    uint32_t GetVertexCount();
    uint32_t GetIndexCount();

};

} // namespace Rendering
