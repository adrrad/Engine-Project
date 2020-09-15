#include "gui/SceneInspector.hpp"
#include "core/Scene.hpp"
#include "platform/WindowManager.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

using namespace std;
using namespace Engine::Core;

namespace Engine::GUI
{
SceneInspector::SceneInspector()
{
    windowManager = Platform::WindowManager::GetInstance();
    windowManager->RegisterWindowResizeCallback([&](int w, int h)
    {
        this->windowWidth = w;
        this->windowHeight = h;
    });
}

void SceneInspector::SetScene(Core::Scene* scene)
{
    _scene = scene;
}


void SceneInspector::DrawGameObjectNode(Engine::Core::GameObject* gameObject)
{
    ImGui::Indent(1);
    ImGui::PushID(gameObject);
    bool isSelected = _selectedGO == gameObject;
    bool open = ImGui::TreeNodeEx(gameObject->Name.c_str(), (isSelected ? ImGuiTreeNodeFlags_Selected : 0));
    if (ImGui::BeginPopupContextItem()) {
        // cout << "Did something on " << gameObject->Name << endl;
        ImGui::EndPopup();
    }
    ImGui::PopID();
    if (ImGui::IsItemClicked()) {
        // cout << "Clicked on " << gameObject->Name << endl;
        _selectedGO = gameObject;
    }

    if (ImGui::BeginDragDropSource()) {
        _draggedGO = gameObject;
        // cout << "Dragging object " << gameObject->Name << endl;
        ImGui::Text(gameObject->Name.c_str());
        ImGui::SetDragDropPayload("go", &gameObject->Name, sizeof(int));
        ImGui::EndDragDropSource();
    }
    //TODO: Figure out how to handle drop on window to detach the object from any parent
    if (ImGui::BeginDragDropTarget()) {
        ImGuiDragDropFlags target_flags = 0;
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("go", target_flags))
        {
            // cout << "Dropped object " << _draggedGO->Name << " on " << gameObject->Name << endl;
            _draggedGO->transform.SetParent(&gameObject->transform);
            _draggedGO = nullptr;
        }
        ImGui::EndDragDropTarget();
    }

    if(open)
    {
        for(auto child : gameObject->transform.GetChildren()) DrawGameObjectNode(child->gameObject);
        ImGui::TreePop();
    }
    ImGui::Unindent(1);
}

void SceneInspector::DrawSceneGraph()
{
    glm::vec2 size = {windowWidth, windowHeight};
    ImVec2 s = ImVec2(size.x*0.1f, size.y);
    ImVec2 sMax = ImVec2(s.x*2, s.y);
    ImGui::SetNextWindowPos(ImVec2(0,0));
    // ImGui::SetNextWindowSize(s);
    ImGui::SetNextWindowSizeConstraints(s, sMax);
    ImGui::Begin("Scene Graph");
    for(auto gameObject : _scene->GetGameObjects())
    {
        if(gameObject->transform.GetParent() == nullptr)
            DrawGameObjectNode(gameObject);
    }
    ImGui::End();
}

void SceneInspector::DrawGameObjectInspector()
{
    glm::vec2 size = {windowWidth, windowHeight};
    ImVec2 s = ImVec2(size.x*0.1f, size.y);
    ImVec2 sMax = ImVec2(s.x*2, s.y);
    // ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSizeConstraints(s, sMax);
    ImGui::Begin("Object Inspector", (bool*)0, ImGuiWindowFlags_NoMove);
        ImGui::SetWindowPos(ImVec2(size.x - ImGui::GetWindowSize().x, 0)); //Don't know the window size before begin so doing this
        if(_selectedGO != nullptr)
        {
            GameObject* go = _selectedGO;
            ImGui::PushID(go);
            ImGui::Columns(2, (const char*)0, false);
            ImGui::Text(go->Name.c_str()); // TODO: Center the object name in this column
            ImGui::NextColumn();
            ImGui::Checkbox("Enabled", &go->Enabled);
            ImGui::Columns(1);
            if(ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat3("Position", &go->transform.position[0], 0.05f);
                glm::vec3 euler = go->transform.rotation.ToEuler();
                ImGui::DragFloat3("Rotation", &euler[0], 0.05f);
                go->transform.rotation = glm::quat(glm::radians(euler));
                ImGui::DragFloat3("Scale", &go->transform.scale[0], 0.05f);
                ImGui::TreePop();
            }
            for(auto comp : go->GetComponents())
            {
                ImGui::PushID(comp);
                ImGui::Columns(2, (const char*)0, false);
                bool open = ImGui::TreeNodeEx(comp->Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
                ImGui::NextColumn();
                bool enabled = comp->IsEnabled();
                ImGui::Checkbox("Enabled", &enabled);
                comp->SetEnabled(enabled);
                ImGui::Columns(1);
                if(open)
                {
                    comp->DrawInspectorGUI();
                    ImGui::TreePop();
                }
                ImGui::PopID();    
            }
            ImGui::PopID();
        }
    ImGui::End();
}

void SceneInspector::DrawGUI()
{
    DrawGameObjectInspector();
    DrawSceneGraph();
    
}


} // namespace GUI
