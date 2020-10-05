#pragma once
#include "editor/gui/Payload.hpp"

#include "utilities/StringUtilities.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <string>

namespace Engine::Editor
{




template <class C>
bool DropField(std::string name, C*& value)
{
    std::string TypeName = Utilities::Split(typeid(C).name(), "::").back();
    std::string valueText = value == nullptr ? "None" : TypeName + " object";
    bool changed = false;
    ImGui::InputText(name.c_str(), valueText.data(), valueText.size(), ImGuiInputTextFlags_ReadOnly);
    if (ImGui::BeginDragDropTarget()) 
    {
        ImGuiDragDropFlags target_flags = 0;
        if (const ImGuiPayload* imguipayload = ImGui::AcceptDragDropPayload("go", target_flags))
        {
            Payload<C>* payload = dynamic_cast<Payload<C>*>(IPayload::GetPayload());
            if(payload != nullptr)
            {
                value = payload->Value;
                changed = true;
            }
        }
        ImGui::EndDragDropTarget();
    }
    return changed;
}

} // namespace Engine::Editor
