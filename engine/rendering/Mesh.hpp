#pragma once

#include "EngineComponent.hpp"
#include "rendering/RenderingStructs.hpp"
#include "rendering/RenderingTypedefs.hpp"
#include "geometry/Volume.hpp"
#include "EngineData.hpp"
#include "Resource.hpp"


#include <cstdint>
#include <vector>

namespace Gameplay
{
    class MeshComponent;
}

namespace Rendering
{
class Renderer;
class Texture;

class Mesh : public SubsystemComponent<Renderer>, public Resource
{
friend class Gameplay::MeshComponent;
private:
    uint32_t m_vbo = 0;
    uint32_t m_ebo = 0;
    uint32_t m_vao = 0;
    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
    
    std::vector<Vertex> m_vertices;
    std::vector<u64> m_indices;

    Geometry::Volume* m_boundingVolume;

    static void CalculateTangents(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

    void CalculateBoundingBox(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);

    void CreateBuffers();

    void CreateVAO();
    
public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, AssetID resourceID);

    uint32_t GetVertexCount();

    uint32_t GetIndexCount();

    __forceinline uint32_t GetVBO() { return m_vbo; }
    
    __forceinline uint32_t GetEBO() { return m_ebo; }

    __forceinline uint32_t GetVAO() { return m_vao; }

    std::vector<Vertex>& GetVertices();

    Geometry::Volume* GetBoundingVolume();

    static Mesh* GetQuad();

    static Mesh* GetSkybox();

    static std::vector<std::pair<Mesh*, std::pair<glm::ivec2, glm::ivec2>>> FromHeightmap(ImageData* heightmap, float scale, float maxHeight, float uvscale = 1, int verticesPerSegment = -1);

};

} // namespace Rendering
