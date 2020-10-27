#pragma once
#include "assets/resources/CubemapAsset.hpp"
#include "assets/resources/ImageAsset.hpp"
#include "assets/AssetManager.hpp"
#include "utilities/json/JSON.hpp"
#include "editor/gui/DropField.hpp"

namespace Engine::Assets
{

CubemapImages::CubemapImages(ImageData* right,
                ImageData* left,
                ImageData* top,
                ImageData* bot,
                ImageData* back,
                ImageData* front) 
{
    Right = right;
    Left = left; 
    Top = top; 
    Bot = bot;
    Back = back;
    Front = front;
}

CubemapAsset::CubemapAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{
    using namespace Utilities::JSON;
    Engine::Array<char> content = resourceFile->ReadAll();
    std::string jsonstring = std::string(content.Data(), content.Length);
    auto parsed = ParseJSON(jsonstring);
    if(parsed == nullptr) return;
    auto& jsonobj = *parsed;
    m_frontImage = AssetID(jsonobj[std::string("front")]->String);
    m_backImage = AssetID(jsonobj[std::string("back")]->String);
    m_topImage = AssetID(jsonobj[std::string("top")]->String);
    m_botImage = AssetID(jsonobj[std::string("bot")]->String);
    m_leftImage = AssetID(jsonobj[std::string("left")]->String);
    m_rightImage = AssetID(jsonobj[std::string("right")]->String);
}

void CubemapAsset::Load()
{
    if(IsLoaded()) return;
    AssetManager* mng = AssetManager::GetInstance();
    mng->GetAsset<ImageAsset>(m_frontImage)->Load();
    mng->GetAsset<ImageAsset>(m_backImage)->Load();
    mng->GetAsset<ImageAsset>(m_topImage)->Load();
    mng->GetAsset<ImageAsset>(m_botImage)->Load();
    mng->GetAsset<ImageAsset>(m_leftImage)->Load();
    mng->GetAsset<ImageAsset>(m_rightImage)->Load();
}

void CubemapAsset::Free()
{

}

void CubemapAsset::Write()
{
    using namespace Utilities::JSON;
    auto obj = JSONValue::AsObject({});
    auto front = JSONValue::AsString(m_frontImage.ToString());
    auto back = JSONValue::AsString(m_backImage.ToString());
    auto top = JSONValue::AsString(m_topImage.ToString());
    auto bot = JSONValue::AsString(m_botImage.ToString());
    auto left = JSONValue::AsString(m_leftImage.ToString());
    auto right = JSONValue::AsString(m_rightImage.ToString());
    obj->Members.push_back(JSONKeyValuePair("front", front));
    obj->Members.push_back(JSONKeyValuePair("back", back));
    obj->Members.push_back(JSONKeyValuePair("top", top));
    obj->Members.push_back(JSONKeyValuePair("bot", bot));
    obj->Members.push_back(JSONKeyValuePair("left", left));
    obj->Members.push_back(JSONKeyValuePair("right", right));
    auto jsonString = obj->ToString();
    ResourceFile->Open(Platform::IO::File::WRITE);
    ResourceFile->Write(jsonString.c_str());
    ResourceFile->Close();
}

bool CubemapAsset::IsLoaded()
{
    AssetManager* mng = AssetManager::GetInstance();
    return mng->IsLoaded(m_frontImage)
        && mng->IsLoaded(m_backImage)
        && mng->IsLoaded(m_topImage)
        && mng->IsLoaded(m_botImage)
        && mng->IsLoaded(m_leftImage)
        && mng->IsLoaded(m_rightImage);
}

void CubemapAsset::EditorGUI()
{
    Editor::DropField<ImageAsset>("Front", m_frontImage);
    Editor::DropField<ImageAsset>("Back", m_backImage);
    Editor::DropField<ImageAsset>("Top", m_topImage);
    Editor::DropField<ImageAsset>("Bot", m_botImage);
    Editor::DropField<ImageAsset>("Left", m_leftImage);
    Editor::DropField<ImageAsset>("Right", m_rightImage);
}

ImageAsset* CubemapAsset::GetSide(CubemapSide side)
{
    AssetManager* mng = AssetManager::GetInstance();
    switch (side)
    {
    case RIGHT:
        return mng->GetAsset<ImageAsset>(m_rightImage);
    case LEFT:
        return mng->GetAsset<ImageAsset>(m_leftImage);
    case TOP:
        return mng->GetAsset<ImageAsset>(m_topImage);
    case BOT:
        return mng->GetAsset<ImageAsset>(m_botImage);
    case BACK:
        return mng->GetAsset<ImageAsset>(m_backImage);
    case FRONT:
        return mng->GetAsset<ImageAsset>(m_frontImage);
    default:
        throw EngineException("Unknown Cubemap side: " + side);
    }
}

CubemapImages CubemapAsset::GetImages()
{
    return CubemapImages(
        GetSide(RIGHT)->GetImageData(),
        GetSide(LEFT)->GetImageData(),
        GetSide(TOP)->GetImageData(),
        GetSide(BOT)->GetImageData(),
        GetSide(BACK)->GetImageData(),
        GetSide(FRONT)->GetImageData()
    );
}

} // namespace Engine::Assets
