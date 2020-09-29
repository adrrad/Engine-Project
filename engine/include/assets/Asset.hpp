#pragma once

#include "platform/io/File.hpp"
#include "utilities/serialisation/Serialisation.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace Engine::Assets
{

class Asset
{

private:
public:
    const AssetID ID;
    Platform::IO::File* ResourceFile;

    /**
     * @brief Construct a new Asset object, represeting the project resource file.
     * 
     * @param resourceFile The project resource file.
     */
    Asset(Platform::IO::File* resourceFile, AssetID id) : ResourceFile(resourceFile), ID(id) {}

    /**
     * @brief Loads the asset data to memory.
     * 
     */
    virtual void Load() = 0;

    /**
     * @brief Frees the memory used for the asset data, making it invalid.
     * 
     */
    virtual void Free() = 0;

    /**
     * @brief Checks whether the asset is loaded into memory.
     * 
     * @return true The asset is loaded to memory.
     * @return false The asset is not loaded to memory.
     */
    virtual bool IsLoaded() = 0;

};

    
} // namespace Engine::Assets
