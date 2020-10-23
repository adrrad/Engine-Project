#include "editor/SceneInspector.hpp"
#include "editor/gui/GUIProperties.hpp"
#include "editor/gui/DropField.hpp"

#include "core/Scene.hpp"
#include "core/GameObject.hpp"

#include "assets/AssetManager.hpp"
#include "assets/resources/ImageAsset.hpp"
#include "assets/resources/CubemapAsset.hpp"
#include "assets/resources/MeshAsset.hpp"
#include "assets/resources/ScriptAsset.hpp"
#include "assets/resources/ShaderAsset.hpp"
#include "assets/resources/FragmentShaderAsset.hpp"
#include "assets/resources/VertexShaderAsset.hpp"
#include "assets/resources/JSONAsset.hpp"
#include "assets/resources/FontAsset.hpp"

#include "platform/io/Path.hpp"

#include "rendering/Renderer.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

using namespace std;
using namespace Engine::Core;

namespace Engine::Editor
{

void SceneInspector::SetAssetPayload(Engine::Platform::IO::File* assetFile)
{
    ImGui::SetDragDropPayload("go", &assetFile->GetName(), sizeof(int));
    Assets::Asset* asset = m_assetManager->GetAsset<Assets::Asset>(
        m_assetManager->GetFilesystem()->GetRelativePath(assetFile->GetPath()));
    
    if(auto ass = dynamic_cast<Assets::ImageAsset*>(asset)) IPayload::SetPayload(ass);
    else if(auto ass = dynamic_cast<Assets::CubemapAsset*>(asset)) IPayload::SetPayload(ass);
    else if(auto ass = dynamic_cast<Assets::MeshAsset*>(asset)) IPayload::SetPayload(ass);
    else if(auto ass = dynamic_cast<Assets::ScriptAsset*>(asset)) IPayload::SetPayload(ass);
    else if(auto ass = dynamic_cast<Assets::ShaderAsset*>(asset)) IPayload::SetPayload(ass);
    else if(auto ass = dynamic_cast<Assets::FragmentShaderAsset*>(asset)) IPayload::SetPayload(ass);
    else if(auto ass = dynamic_cast<Assets::VertexShaderAsset*>(asset)) IPayload::SetPayload(ass);
    else if(auto ass = dynamic_cast<Assets::JSONAsset*>(asset)) IPayload::SetPayload(ass);
    // else if(auto ass = dynamic_cast<Assets::MeshAsset*>(asset)) IPayload::SetPayload(ass);
}

uint32_t GetTextureID(std::string relativePath)
{
    auto img = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>(Platform::IO::Path(relativePath));
    return Rendering::Renderer::GetInstance()->GetTexture(img->ID)->GetID();
}

void SceneInspector::LoadFonts()
{
    Assets::FontAsset* consola = m_assetManager->GetAsset<Assets::FontAsset>("fonts/consola.ttf");
    GUIProperties::LoadFont("consola", consola->ID);
}

SceneInspector::SceneInspector() 
    : filesPanel(Assets::AssetManager::GetInstance()->GetFilesystem(), "Assets", 0, 0, 0, 0,
        GetTextureID("icons/fileIcon.jpg"), GetTextureID("icons/folder.png"), GetTextureID("icons/folderup.png"))
{
    m_assetManager = Assets::AssetManager::GetInstance();
    m_windowManager = Platform::WindowManager::GetInstance();
    m_windowManager->RegisterWindowResizeCallback([&](int w, int h)
    {
        GUIProperties::WindowWidth = w;
        GUIProperties::WindowHeight = h;
        rightPanel.MinWidth = uint32_t(w * 0.1f);
        rightPanel.MaxWidth = uint32_t(w * 0.2f);
        leftPanel.MinWidth = uint32_t(w * 0.1f);
        leftPanel.MaxWidth = uint32_t(w * 0.2f);
        topPanel.MinHeight = uint32_t(h*0.1);
        topPanel.MaxHeight = uint32_t(h*0.2);
        filesPanel.MinHeight = uint32_t(h*0.1);
        filesPanel.MaxHeight = uint32_t(h*0.2);
    });
    LoadFonts();
    
}

void SceneInspector::SetScene(Core::Scene* scene)
{
    m_scene = scene;
}

void SceneInspector::DrawGameObjectNode(Engine::Core::GameObject* gameObject)
{
    ImGui::Indent(1);
    ImGui::PushID(gameObject);
    bool isSelected = GetSelectedItem<GameObject>() == gameObject;
    bool hasChildren = gameObject->transform.GetChildren().size() > 0;
    ImGuiTreeNodeFlags flags = 
        (isSelected ? ImGuiTreeNodeFlags_Selected : 0) | (hasChildren ? 0 : ImGuiTreeNodeFlags_Leaf);
    bool open = ImGui::TreeNodeEx(gameObject->Name.c_str(), flags);
    if (ImGui::BeginPopupContextItem()) {
        ImGui::EndPopup();
    }
    ImGui::PopID();
    if (ImGui::IsItemClicked()) {
        SelectItem(gameObject);
    }

    if (ImGui::BeginDragDropSource()) {
        m_draggedGO = gameObject;
        ImGui::Text(gameObject->Name.c_str());
        ImGui::SetDragDropPayload("go", &gameObject->Name, sizeof(int));
        IPayload::SetPayload(gameObject);
        ImGui::EndDragDropSource();
    }
    
    if (ImGui::BeginDragDropTarget()) 
    {
        ImGuiDragDropFlags target_flags = 0;
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("go", target_flags))
        {
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

void SceneInspector::DrawAssetInspector()
{
    auto asset = GetSelectedItem<Assets::Asset>();
    ImGui::Text(asset->ResourceFile->GetName().c_str());
    ImGui::Columns(2, 0, false);
    bool save = ImGui::Button("Save");
    ImGui::Columns(1);
    asset->EditorGUI();
    if(save) asset->Write();
}

void SceneInspector::DrawGameObjectInspector()
{
    static Core::GameObject* TEST = nullptr;
    GameObject* go = GetSelectedItem<GameObject>();
    if(go != nullptr)
    {
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
    std::string dirname = dir->GetPath().GetDirname();
    ImGui::PushID(dir);
    bool open = ImGui::TreeNodeEx(dirname.c_str());
    if(open)
    {
        ImGui::Indent();
        for(auto& subdir : dir->GetSubdirectories()) DrawDirectoryContent(&subdir);
        for(auto& file : dir->Files)
        {
            bool open = ImGui::TreeNodeEx(file.GetFilename().c_str(), ImGuiTreeNodeFlags_Leaf);
            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0))
            {
                SelectItem(m_assetManager->GetAsset<Assets::Asset>(
                    m_assetManager->GetFilesystem()->GetRelativePath(file)
                ));
            }
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("go", &file.GetFilename(), sizeof(int));
                SetAssetPayload(m_assetManager->GetFilesystem()->GetFile(file));
                ImGui::EndDragDropSource();
            }
            if(open) ImGui::TreePop();
        }
        ImGui::Unindent();
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void SceneInspector::DrawProjectFiles()
{
    DrawDirectoryContent(m_assetManager->GetFilesystem()->GetRootDirectory());
    ImVec2 mpos = ImGui::GetMousePos();
    if(ImGui::IsMouseReleased(1)) std::cout << "clicked outside" << std::endl;
    if(leftPanel.ContainsPoint(mpos.x, mpos.y)) 
    {
        if(ImGui::IsMouseReleased(1)) std::cout << "clicked inside" << std::endl;
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("New Cubemap"))
            {
                Platform::IO::Path path("/skybox.cubemap");
                m_assetManager->CreateAsset<Assets::CubemapAsset>(path);
            }

            ImGui::EndPopup();
        }
    }
}

void SceneInspector::DrawRightPanel()
{
    rightPanel.MinHeight = rightPanel.MaxHeight = GUIProperties::WindowHeight - filesPanel.Height;
    rightPanel.Begin();
    if(GetSelectedItem<GameObject>()) DrawGameObjectInspector();
    else if(GetSelectedItem<Assets::Asset>()) DrawAssetInspector();
    rightPanel.End();
}

void SceneInspector::DrawLeftPanel()
{
    leftPanel.MinHeight = leftPanel.MaxHeight = GUIProperties::WindowHeight - filesPanel.Height;
    leftPanel.Begin();
        DrawSceneGraph();
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

void SceneInspector::DrawBottomPanel()
{
    filesPanel.X = 0;
    filesPanel.MinWidth = filesPanel.MaxWidth = GUIProperties::WindowWidth;
    filesPanel.Y = GUIProperties::WindowHeight - filesPanel.Height;
    filesPanel.Draw();
}

void SceneInspector::DrawGUI()
{
    DrawRightPanel();
    DrawLeftPanel();
    DrawTopPanel();
    DrawBottomPanel();
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
