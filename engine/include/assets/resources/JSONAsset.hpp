#pragma once
#include "assets/Asset.hpp"
#include "utilities/json/JSON.hpp"

namespace Engine::Assets
{

class JSONAsset : public Asset
{
private:
    std::shared_ptr<Utilities::JSON::JSONValue> m_json;
public:

    JSONAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

    std::shared_ptr<Utilities::JSON::JSONValue> GetJSONObject();
};

    
} // namespace Engine::Assets
