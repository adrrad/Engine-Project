#pragma once
#include "Panel.hpp"
#include "platform/io/Filesystem.hpp"
#include "editor/gui/SelectableItem.hpp"
#include "imgui_internal.h"

namespace Engine::Editor
{


class FilesPanel : public Panel
{

private:

    Platform::IO::Filesystem* m_filesystem;
    Platform::IO::Directory* m_currentDir;
    SelectableItem* m_selectedItem;
    uint64_t m_fileIconID, m_folderIconID, m_folderUpIconID;
    float m_iconSizepx = 50;
    
    inline bool DrawDirectory(Platform::IO::Directory* dir);

    inline bool DrawFile(Platform::IO::File* file);

    inline void DrawCurrentDirectory();

public:

    inline FilesPanel(Platform::IO::Filesystem* filesystem, std::string name, 
                    uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                    uint32_t fileIconID, uint32_t folderIconID, uint32_t folderUpIconID);

    inline void Draw();

};


FilesPanel::FilesPanel(Platform::IO::Filesystem* filesystem, std::string name, 
                    uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
                    uint32_t fileIconID, uint32_t folderIconID, uint32_t folderUpIconID)
                : Panel(name, x, y, width, height)
{
    m_filesystem = filesystem;
    m_currentDir = filesystem->GetRootDirectory();
    m_fileIconID = fileIconID;
    m_folderIconID = folderIconID;
    m_folderUpIconID = folderUpIconID;
}

bool FilesPanel::DrawDirectory(Platform::IO::Directory* dir)
{
    ImGui::PushStyleColor(ImGuiCol_Button, {1,1,1,0});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1,1,1,0});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1,1,1,0});
    ImGui::PushID(dir);
    ImGui::BeginGroup();
    bool clicked = ImGui::ImageButton(ImTextureID(m_folderIconID), {m_iconSizepx, m_iconSizepx});
    std::string name = dir->Name;
    if(name.length() > 6) name = name.substr(0, 6) + "...";
    ImGui::Text(name.c_str());
    ImGui::EndGroup();
    ImGui::PopID();
    ImGui::PopStyleColor(3);
    return clicked;
}

bool FilesPanel::DrawFile(Platform::IO::File* file)
{
    ImGui::PushStyleColor(ImGuiCol_Button, {1,1,1,0});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1,1,1,0});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1,1,1,0});
    ImGui::PushID(file);
    ImGui::BeginGroup();
    bool clicked = ImGui::ImageButton(ImTextureID(m_fileIconID), {m_iconSizepx, m_iconSizepx});
    ImGui::Text(file->FileName.c_str());
    ImGui::EndGroup();
    ImGui::PopID();
    ImGui::PopStyleColor(3);
    return clicked;
}

void FilesPanel::DrawCurrentDirectory()
{
    uint32_t numColumns = Width / uint32_t(m_iconSizepx);
    uint32_t itemIndex = 0;
    
    bool isRoot = m_currentDir == m_filesystem->GetRootDirectory();

    bool goUp = ImGui::ImageButton(ImTextureID(m_folderUpIconID), {m_iconSizepx, m_iconSizepx});
    if(goUp)
    {
        m_currentDir = new Platform::IO::Directory(m_currentDir->GetParentDirectory());
    }
    itemIndex++;

    for(auto& subdir : m_currentDir->Subdirectories)
    {
        if(itemIndex % numColumns) ImGui::SameLine();
        bool clicked = DrawDirectory(&subdir);
        if(clicked)
        {
            m_currentDir = &subdir;
            return;
        }
        itemIndex++;
    }

    for(auto& file : m_currentDir->Files)
    {
        if(itemIndex % numColumns) ImGui::SameLine();
        bool clicked = DrawFile(&file);
        itemIndex++;
    }
}

void FilesPanel::Draw()
{
    Name = m_currentDir->DirectoryPath.ToString();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.5, 0.5, 0.5, 0.5});
    Panel::Begin();
        DrawCurrentDirectory();
    Panel::End();
    ImGui::PopStyleColor();
}

} // namespace Engine::Editor
