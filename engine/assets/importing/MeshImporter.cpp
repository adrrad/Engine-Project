#include "assets/importing/MeshImporter.hpp"

// #define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Assets::Importing
{

MeshData* MeshImporter::ImportMesh(Platform::IO::File* meshFile)
{
    Array<char> objData = meshFile->ReadAll();
    tinyobj::ObjReader reader;
    reader.ParseFromString(objData.Data(), "");
    auto meshes = reader.GetShapes();
    auto attrib = reader.GetAttrib();
    auto pos = attrib.vertices;
    auto norms = attrib.normals;
    auto UVs = attrib.texcoords;

    // auto pos = verts;
    // auto norms = std::vector<float>(normals.size(), normals.data());
    // auto UVs = std::vector<float>(uvs.size(), uvs.data());
    auto inds = std::vector<MeshIndices>(meshes.size());

    for(Index meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
    {
        auto indices = meshes[meshIndex].mesh.indices;
        auto posIndices = std::vector<Index>(indices.size());
        auto normIndices = std::vector<Index>(indices.size());
        auto uvIndices = std::vector<Index>(indices.size());
        for(Index i = 0; i < indices.size(); i++)
        {
            posIndices[i] = indices[i].vertex_index;
            normIndices[i] = indices[i].normal_index;
            uvIndices[i] = indices[i].texcoord_index;
        }
        inds[meshIndex] = MeshIndices(posIndices, normIndices, uvIndices);
    }
    return new MeshData(pos, norms, UVs, inds);
}

} // namespace Assets::Importing
