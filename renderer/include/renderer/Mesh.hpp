#pragma once
#include "renderer/RenderingStructs.hpp"
#include "Shader.hpp"

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
    uint32_t _vertexCount = 0;
    uint32_t _indexCount = 0;
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, Shader* shader);

    static Mesh* GetParticlePlane(uint32_t length, uint32_t width, Shader* shader, float scale = 1);

    static Mesh* GetQuad(Shader* shader);

    static Mesh* GetSkybox(Shader* shader);
    
    static Mesh* GetCube(Shader* shader);

    static Mesh* GetSphere(Shader* shader);

    static Mesh* FromFile(std::string path, Shader* shader);

    uint32_t GetVAO();
    uint32_t GetVertexCount();
    uint32_t GetIndexCount();

};

} // namespace Rendering
