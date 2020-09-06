#pragma once
#include "renderer/RenderingStructs.hpp"
#include "renderer/RenderingTypedefs.hpp"
#include "geometry/Volume.hpp"
#include <cstdint>
#include <vector>

namespace Components
{
    class MeshComponent;
}

namespace Rendering
{

class Mesh
{
friend class Components::MeshComponent;
private:
    uint32_t _vbo = 0;
    uint32_t _ebo = 0;
    uint32_t _vertexCount = 0;
    uint32_t _indexCount = 0;
    
    Engine::Geometry::Volume* _boundingVolume;

    static void CalculateTangents(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

    
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    static Mesh* GetPlane(uint32_t length, uint32_t width, float scale = 1);

    static Mesh* GetQuad();

    static Mesh* GetSkybox();
    
    static Mesh* GetCube();

    static Mesh* GetSphere();

    static Mesh* FromFile(std::string path);

    static Mesh* FromHeightmap(std::string path, float scale, float maxHeight, float uvscale = 1);

    uint32_t GetVertexCount();

    uint32_t GetIndexCount();

};

} // namespace Rendering
