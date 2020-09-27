#pragma once

#include "EngineData.hpp"
#include "platform/io/File.hpp"

namespace Engine::Assets::Importing
{
    
class MeshImporter
{

/**
 * @brief Imports a mesh model from a file.
 * 
 * @param meshFile The file containing mesh data
 * @return MeshData* A pointer to a mesh data struct.
 */
static MeshData* ImportMesh(Platform::IO::File* meshFile);

};

} // namespace Engine::Assets::Importing
