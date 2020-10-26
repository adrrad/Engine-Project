#pragma once
#include "editor/gui/Payload.hpp"
#include "assets/AssetManager.hpp"

#include "utilities/StringUtilities.hpp"

#include "EngineTypedefs.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>
#include <functional>

namespace Engine::Editor
{

template<typename T>
using Getter = std::function<T*()>;

template<typename T>
using Setter = std::function<void(T*)>;

template <class C>
bool DropField(std::string name, C*& value)
{
    static auto assetManager = Assets::AssetManager::GetInstance();
    std::string TypeName = Utilities::Split(typeid(C).name(), "::").back();
    std::string valueText = value == nullptr ? "None" : TypeName + " object";
    bool changed = false;
    ImGui::InputText(name.c_str(), valueText.data(), valueText.size(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) 
    {
        ImGuiDragDropFlags target_flags = 0;
        if (const ImGuiPayload* imguipayload = ImGui::AcceptDragDropPayload("go", target_flags))
        {
            Payload<Platform::IO::File>* filePayload = dynamic_cast<Payload<Platform::IO::File>*>(IPayload::GetPayload());
            if(filePayload != nullptr) 
            {
                C* asset = assetManager->GetAsset<C>(filePayload->Value->GetPath());
                value = asset;
                changed = true;
            }
            else
            {
                Payload<C>* payload = dynamic_cast<Payload<C>*>(IPayload::GetPayload());
                if(payload != nullptr)
                {
                    value = payload->Value;
                    changed = true;
                }
            }

        }
        ImGui::EndDragDropTarget();
    }
    return changed;
}


template <class C>
bool DropField(std::string name, Getter<C> get, Setter<C> set)
{
    static auto assetManager = Assets::AssetManager::GetInstance();
    std::string TypeName = Utilities::Split(typeid(C).name(), "::").back();
    std::string valueText = get() == nullptr ? "None" : TypeName + " object";
    bool changed = false;
    
    ImGui::InputText(name.c_str(), valueText.data(), valueText.size(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) 
    {
        ImGuiDragDropFlags target_flags = 0;
        if (const ImGuiPayload* imguipayload = ImGui::AcceptDragDropPayload("go", target_flags))
        {
            Payload<Platform::IO::File>* filePayload = dynamic_cast<Payload<Platform::IO::File>*>(IPayload::GetPayload());
            if(filePayload != nullptr) 
            {
                C* asset = assetManager->GetAsset<C>(filePayload->Value->GetPath());
                set(asset);
                changed = true;
            }
            else
            {
                Payload<C>* payload = dynamic_cast<Payload<C>*>(IPayload::GetPayload());
                if(payload != nullptr)
                {
                    set(payload->Value);
                    changed = true;
                }
            }

        }
        ImGui::EndDragDropTarget();
    }
    return changed;
}

template <class C>
bool DropField(std::string name, AssetID& id)
{
    static auto assetManager = Assets::AssetManager::GetInstance();
    std::string TypeName = Utilities::Split(typeid(C).name(), "::").back();
    std::string valueText = "None";
    if(id.IsAssigned())
    {
        valueText = assetManager->GetAsset<Assets::Asset>(id)->ResourceFile->GetName();
    }
    ImGui::InputText(name.c_str(), valueText.data(), valueText.size(), ImGuiInputTextFlags_ReadOnly);
    bool changed = false;

    if (ImGui::BeginDragDropTarget())
    {
        ImGuiDragDropFlags target_flags = 0;
        if (const ImGuiPayload* imguipayload = ImGui::AcceptDragDropPayload("go", target_flags))
        {
            Payload<Platform::IO::File>* filePayload = dynamic_cast<Payload<Platform::IO::File>*>(IPayload::GetPayload());
            if(filePayload != nullptr) 
            {
                C* asset = assetManager->GetAsset<C>(filePayload->Value->GetPath());
                id = asset->ID;
                changed = true;
            }
            else
            {
                Payload<C>* payload = dynamic_cast<Payload<C>*>(IPayload::GetPayload());
                if(payload != nullptr)
                {
                    id = payload->Value->ID;
                    changed = true;
                }
            }

        }
        ImGui::EndDragDropTarget();
    }
    return changed;
}

} // namespace Engine::Editor
