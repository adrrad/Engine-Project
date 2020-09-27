#include "assets/importing/MeshImporter.hpp"

// #define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Engine::Assets::Importing
{

MeshData* MeshImporter::ImportMesh(Platform::IO::File* meshFile)
{
    Array<char> objData = meshFile->ReadAll();
    tinyobj::ObjReader reader;
    reader.ParseFromString(objData.Data(), "");
    auto meshes = reader.GetShapes();
    auto attrib = reader.GetAttrib();
    auto verts = attrib.vertices;
    auto normals = attrib.normals;
    auto uvs = attrib.texcoords;

    auto pos = Array<float>(verts.size(), verts.data(), true);
    auto norms = Array<float>(normals.size(), normals.data(), true);
    auto UVs = Array<float>(uvs.size(), uvs.data(), true);
    auto inds = Array<MeshIndices>(meshes.size());

    for(Index meshIndex = 0; meshIndex < meshes.size(); meshIndex++)
    {
        auto indices = meshes[meshIndex].mesh.indices;
        auto posIndices = Array<Index>(indices.size());
        auto normIndices = Array<Index>(indices.size());
        auto uvIndices = Array<Index>(indices.size());
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

} // namespace Engine::Assets::Importing
