#include "editor/SceneInspector.hpp"
#include "editor/gui/GUIProperties.hpp"

#include "core/Scene.hpp"
#include "core/GameObject.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

using namespace std;
using namespace Engine::Core;

namespace Engine::Editor
{
SceneInspector::SceneInspector()
{
    m_assetManager = Assets::AssetManager::GetInstance();
    m_windowManager = Platform::WindowManager::GetInstance();
    m_windowManager->RegisterWindowResizeCallback([&](int w, int h)
    {
        GUIProperties::WindowWidth = w;
        GUIProperties::WindowHeight = h;
        rightPanel.MinHeight = rightPanel.MaxHeight = h;
        rightPanel.MinWidth = uint32_t(w * 0.1f);
        rightPanel.MaxWidth = uint32_t(w * 0.2f);
        leftPanel.MinHeight = leftPanel.MaxHeight = h;
        leftPanel.MinWidth = uint32_t(w * 0.1f);
        leftPanel.MaxWidth = uint32_t(w * 0.2f);
        topPanel.MinHeight = uint32_t(h*0.1);
        topPanel.MaxHeight = uint32_t(h*0.2);
    });
}

void SceneInspector::SetScene(Core::Scene* scene)
{
    m_scene = scene;
}

void SceneInspector::DrawGameObjectNode(Engine::Core::GameObject* gameObject)
{
    ImGui::Indent(1);
    ImGui::PushID(gameObject);
    bool isSelected = m_selectedGO == gameObject;
    bool hasChildren = gameObject->transform.GetChildren().size() > 0;
    ImGuiTreeNodeFlags flags = 
        (isSelected ? ImGuiTreeNodeFlags_Selected : 0) | (hasChildren ? 0 : ImGuiTreeNodeFlags_Leaf);
    bool open = ImGui::TreeNodeEx(gameObject->Name.c_str(), flags);
    if (ImGui::BeginPopupContextItem()) {
        // cout << "Did something on " << gameObject->Name << endl;
        ImGui::EndPopup();
    }
    ImGui::PopID();
    if (ImGui::IsItemClicked()) {
        // cout << "Clicked on " << gameObject->Name << endl;
        m_selectedGO = gameObject;
    }

    if (ImGui::BeginDragDropSource()) {
        m_draggedGO = gameObject;
        // cout << "Dragging object " << gameObject->Name << endl;
        ImGui::Text(gameObject->Name.c_str());
        ImGui::SetDragDropPayload("go", &gameObject->Name, sizeof(int));
        ImGui::EndDragDropSource();
    }
    //TODO: Figure out how to handle drop on window to detach the object from any parent
    if (ImGui::BeginDragDropTarget()) 
    {
        ImGuiDragDropFlags target_flags = 0;
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("go", target_flags))
        {
            // cout << "Dropped object " << m_draggedGO->Name << " on " << gameObject->Name << endl;
            m_draggedGO->transform.SetParent(&gameObject->transform);
            m_draggedGO = nullptr;
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
    for(auto gameObject : m_scene->GetGameObjects())
    {
        if(gameObject->transform.GetParent() == nullptr)
            DrawGameObjectNode(gameObject);
    }
}

void SceneInspector::DrawGameObjectInspector()
{
    if(m_selectedGO != nullptr)
    {
        GameObject* go = m_selectedGO;
        ImGui::PushID(go);
        ImGui::Columns(3, (const char*)0, false);
        ImGui::Text(go->Name.c_str());
        ImGui::NextColumn();
        ImGui::Checkbox("Enabled", &go->m_enabled);
        ImGui::NextColumn();
        bool isStatic = go->Static();
        ImGui::Checkbox("Static", &isStatic);
        if(isStatic != go->Static()) go->SetStatic(isStatic);
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
        for(auto pair : go->GetComponents())
        {
            auto comp = pair.second;
            ImGui::PushID(comp);
            ImGui::Columns(2, (const char*)0, false);
            bool open = ImGui::TreeNodeEx(comp->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen);
            ImGui::NextColumn();
            bool enabled = comp->Enabled();
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
}

void SceneInspector::DrawControlPanel()
{
    ImGui::Columns(3, (const char*)0, false);
    ImGui::NextColumn();
    if(ImGui::Button("Play")) playCallback();
    ImGui::SameLine();
    if(ImGui::Button("Pause")) pauseCallback();
    ImGui::SameLine();
    if(ImGui::Button("Stop")) stopCallback();
}

void SceneInspector::DrawDirectoryContent(Platform::IO::Directory* dir)
{
    std::string dirname = dir->DirectoryPath.GetDirname();
    ImGui::PushID(dir);
    bool open = ImGui::TreeNodeEx(dirname.c_str());
    if(open)
    {
        ImGui::Indent();
        for(auto& subdir : dir->Subdirectories) DrawDirectoryContent(&subdir);
        for(auto& file : dir->Files)
        {
            if(ImGui::TreeNodeEx(file.FileName.c_str(), ImGuiTreeNodeFlags_Leaf)) ImGui::TreePop();
        } 
        ImGui::Unindent();
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void SceneInspector::DrawProjectFiles()
{
    DrawDirectoryContent(m_assetManager->GetFilesystem()->GetRootDirectory());
}

void SceneInspector::DrawRightPanel()
{
    rightPanel.Begin();
        DrawGameObjectInspector();
    rightPanel.End();
}

void SceneInspector::DrawLeftPanel()
{
    static bool showAssets = false;
    leftPanel.Begin();
        if(ImGui::Button("Scene Objects", ImVec2(leftPanel.Width*0.5f, 25.0f))) showAssets = false;
        ImGui::SameLine(0,0);
        if(ImGui::Button("Assets", ImVec2(leftPanel.Width*0.5f, 25.0f))) showAssets = true;
        if(!showAssets) DrawSceneGraph();
        else DrawProjectFiles();
    leftPanel.End();
}

void SceneInspector::DrawTopPanel()
{
    topPanel.X = leftPanel.Width;
    topPanel.MinWidth = topPanel.MaxWidth = GUIProperties::WindowWidth - ((leftPanel.Width - leftPanel.X) + (rightPanel.Width - rightPanel.X));
    topPanel.Begin();
        DrawControlPanel();
    topPanel.End();
}

void SceneInspector::DrawGUI()
{
    DrawRightPanel();
    DrawLeftPanel();
    DrawTopPanel();
}

void SceneInspector::SetPlayCallback(std::function<void()> callback)
{
    playCallback = callback;
}

void SceneInspector::SetPauseCallback(std::function<void()> callback)
{
    pauseCallback = callback;
}

void SceneInspector::SetStopCallback(std::function<void()> callback)
{
    stopCallback = callback;
}



} // namespace GUI
