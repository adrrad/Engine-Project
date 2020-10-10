#pragma once


#include "rendering/RenderingStructs.hpp"
#include "rendering/RenderingTypedefs.hpp"
#include "geometry/Volume.hpp"
#include "EngineData.hpp"
#include "Resource.hpp"


#include <cstdint>
#include <vector>

namespace Engine::Components
{
    class MeshComponent;
}

namespace Engine::Rendering
{
class Texture;

class Mesh : public Resource
{
friend class Components::MeshComponent;
private:
    uint32_t _vbo = 0;
    uint32_t _ebo = 0;
    uint32_t _vertexCount = 0;
    uint32_t _indexCount = 0;
    
    std::vector<Vertex> m_vertices;
    std::vector<Index> m_indices;

    Engine::Geometry::Volume* _boundingVolume;

    static void CalculateTangents(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

    void CalculateBoundingBox(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

    void CreateBuffers();
    
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, AssetID resourceID);

    uint32_t GetVertexCount();

    uint32_t GetIndexCount();

    __forceinline uint32_t GetVBO() { return _vbo; }
    
    __forceinline uint32_t GetEBO() { return _ebo; }

    std::vector<Vertex>& GetVertices();

    Engine::Geometry::Volume* GetBoundingVolume();

    static Mesh* GetQuad();

    static Mesh* GetSkybox();

    static std::vector<std::pair<Mesh*, std::pair<glm::ivec2, glm::ivec2>>> FromHeightmap(ImageData* heightmap, float scale, float maxHeight, float uvscale = 1, int verticesPerSegment = -1);

};

} // namespace Engine::Rendering
