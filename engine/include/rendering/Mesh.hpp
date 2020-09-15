#pragma once
#include "rendering/RenderingStructs.hpp"
#include "rendering/RenderingTypedefs.hpp"
#include "geometry/Volume.hpp"
#include <cstdint>
#include <vector>

namespace Engine::Components
{
    class MeshComponent;
}

namespace Engine::Rendering
{
class Texture;

class Mesh
{
friend class Components::MeshComponent;
private:
    uint32_t _vbo = 0;
    uint32_t _ebo = 0;
    uint32_t _vertexCount = 0;
    uint32_t _indexCount = 0;
    
    std::vector<Vertex> _vertices;

    Engine::Geometry::Volume* _boundingVolume;

    static void CalculateTangents(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

    void CalculateBoundingBox(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);

    uint32_t GetVertexCount();

    uint32_t GetIndexCount();

    std::vector<Vertex>& GetVertices();

    Engine::Geometry::Volume* GetBoundingVolume();

    static Mesh* GetPlane(uint32_t length, uint32_t width, float scale = 1);

    static Mesh* GetQuad();

    static Mesh* GetSkybox();
    
    static Mesh* GetCube();

    static Mesh* GetSphere();

    static Mesh* FromFile(std::string path);

    static std::vector<std::pair<Mesh*, std::pair<glm::ivec2, glm::ivec2>>> FromHeightmap(Texture* heightmap, float scale, float maxHeight, float uvscale = 1, int verticesPerSegment = -1);

};

} // namespace Engine::Rendering