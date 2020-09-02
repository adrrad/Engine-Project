#include "gui/SceneInspector.hpp"
#include "core/Scene.hpp"
#include "renderer/Renderer.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

using namespace std;

namespace Engine::GUI
{

void SceneInspector::SetScene(Core::Scene* scene)
{
    _scene = scene;
}

auto renderer = Rendering::Renderer::GetInstance();

void SceneInspector::DrawGameObjectNode(Engine::Core::GameObject* gameObject)
{
    ImGui::Indent(1);
    ImGui::PushID(gameObject);
    bool open = ImGui::TreeNodeEx(gameObject->Name.c_str());
    if (ImGui::BeginPopupContextItem()) {
        cout << "Crapped on " << gameObject->Name << endl;
        ImGui::EndPopup();
    }
    ImGui::PopID(); 
    if (ImGui::IsItemClicked()) {
        cout << "Clicked on " << gameObject->Name << endl;
    }

    if (ImGui::BeginDragDropSource()) {
        _selectedGO = gameObject;
        cout << "Dragging object " << gameObject->Name << endl;
        ImGui::Text(gameObject->Name.c_str());
        ImGui::SetDragDropPayload("go", &gameObject->Name, sizeof(int));
        ImGui::EndDragDropSource();
    }
    //TODO: Figure out how to handle drop on window to detach the object from any parent
    if (ImGui::BeginDragDropTarget()) {
        ImGuiDragDropFlags target_flags = 0;
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("go", target_flags))
        {
            cout << "Dropped object " << _selectedGO->Name << " on " << gameObject->Name << endl;
            _selectedGO->transform.SetParent(&gameObject->transform);
            _selectedGO = nullptr;
        }

        ImGui::EndDragDropTarget();
    }

    if(open)
    {
        ImGui::DragFloat3("Position", &gameObject->transform.position[0], 0.05f);
        ImGui::DragFloat3("Rotation", &gameObject->transform.rotation[0], 0.05f);
        ImGui::DragFloat3("Scale", &gameObject->transform.scale[0], 0.05f);
        for(auto child : gameObject->transform.GetChildren()) DrawGameObjectNode(child->gameObject);
        ImGui::TreePop();
    }
    ImGui::Unindent(1);
}

void SceneInspector::DrawGUI()
{
    glm::vec2 size = renderer->GetWindowDimensions();
    ImVec2 s = ImVec2(size.x*0.1f, size.y);
    ImVec2 sMax = ImVec2(s.x*2, s.y);
    ImGui::SetNextWindowPos(ImVec2(0,0));
    // ImGui::SetNextWindowSize(s);
    ImGui::SetNextWindowSizeConstraints(s, sMax);
    ImGui::Begin("Scene Inspector");
    for(auto gameObject : _scene->GetGameObjects())
    {
        if(gameObject->transform.GetParent() == nullptr)
            DrawGameObjectNode(gameObject);
    }
    ImGui::End();
}


} // namespace GUI

// ImGui::Begin("Scene Inspector");

// for(auto gameObject : _scene->GetGameObjects())
// {
//     ImGui::PushID(i);
//     if(ImGui::TreeNodeEx(gameObject->Name.c_str()))
//     {
//         ImGui::DragFloat3("Position", &gameObject->transform.position[0], 0.1f);
//         ImGui::DragFloat3("Rotation", &gameObject->transform.rotation[0], 0.1f);
//         ImGui::DragFloat3("Scale", &gameObject->transform.scale[0], 0.1f);
//         ImGui::TreePop();
//     }
//     ImGui::PopID();
//     i++;   
// }
// ImGui::End();