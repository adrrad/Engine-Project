#pragma once
#include "Panel.hpp"
#include "platform/io/Filesystem.hpp"
#include "editor/gui/SelectableItem.hpp"
#include "Payload.hpp"

namespace Engine::Editor
{


class FilesPanel : public Panel
{

private:

    Platform::IO::Filesystem* m_filesystem;
    Platform::IO::Directory m_currentDir;
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
    m_currentDir = *filesystem->GetRootDirectory();
    m_fileIconID = fileIconID;
    m_folderIconID = folderIconID;
    m_folderUpIconID = folderUpIconID;
}

static inline std::string AdjustName(const std::string& name, float maxPixelWidth)
{
    ImFont* font = ImGui::GetFont();
    int charWidth = int(ImGui::CalcTextSize("w").x);
    int maxNumChars = maxPixelWidth/charWidth;
    if(name.length() < maxNumChars) return name;
    return name.substr(0, maxNumChars-3)+"...";
}

bool FilesPanel::DrawDirectory(Platform::IO::Directory* dir)
{
    ImGui::PushStyleColor(ImGuiCol_Button, {1,1,1,0});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1,1,1,0});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1,1,1,0.5});
    std::string name = AdjustName(dir->GetName(), m_iconSizepx);
    float textWidth = ImGui::CalcTextSize(name.c_str()).x;
    float offset = int((m_iconSizepx - textWidth)/2)+5;
    ImGui::PushID(dir->GetPath().ToString().c_str());
    ImGui::BeginGroup();
    bool clicked = ImGui::ImageButton(ImTextureID(m_folderIconID), {m_iconSizepx, m_iconSizepx});
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
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
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1,1,1,0.5});
    std::string name = AdjustName(file->GetName(), m_iconSizepx);
    float textWidth = ImGui::CalcTextSize(name.c_str()).x;
    float offset = int((m_iconSizepx - textWidth)/2)+5;
    ImGui::PushID(file);
    ImGui::BeginGroup();
    bool clicked = ImGui::ImageButton(ImTextureID(m_fileIconID), {m_iconSizepx, m_iconSizepx});
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
    ImGui::Text(name.c_str());
    ImGui::EndGroup();
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::Text(file->GetName().c_str());
        ImGui::SetDragDropPayload("go", &file->GetName(), sizeof(int));
        IPayload::SetPayload(file);
        ImGui::EndDragDropSource();
    }
    ImGui::PopID();
    ImGui::PopStyleColor(3);
    return clicked;
}

void FilesPanel::DrawCurrentDirectory()
{
    uint32_t numColumns = Width / uint32_t(m_iconSizepx);
    uint32_t itemIndex = 0;
    
    // Draw current path
    std::string currentDir = m_currentDir.GetPath().ToString();
    ImGui::Text(currentDir.c_str());
    ImGui::NewLine();
    // Go to parent directory button handling
    bool isRoot = m_currentDir.GetPath() == m_filesystem->GetRootDirectory()->GetPath();
    ImGui::PushStyleColor(ImGuiCol_Button, {1,1,1,0});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {1,1,1,0});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {1,1,1,0.5});
    bool goUp = isRoot ? false : ImGui::ImageButton(ImTextureID(m_folderUpIconID), {m_iconSizepx, m_iconSizepx});
    ImGui::PopStyleColor(3);
    if(goUp)
    {
        m_currentDir = m_currentDir.GetParentDirectory();
    }
    itemIndex++;

    // Directory buttons
    for(auto& subdir : m_currentDir.GetSubdirectories())
    {
        if(itemIndex % numColumns) ImGui::SameLine();
        bool clicked = DrawDirectory(&subdir);
        if(clicked)
        {
            m_currentDir = subdir.GetPath();
            return;
        }
        itemIndex++;
    }

    // File buttons
    for(auto& file : m_currentDir.Files)
    {
        if(itemIndex % numColumns) ImGui::SameLine();
        bool clicked = DrawFile(m_filesystem->GetFile(file));
        itemIndex++;
    }
}

void FilesPanel::Draw()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.5, 0.5, 0.5, 0.5});
    Panel::Begin();
        DrawCurrentDirectory();
    Panel::End();
    ImGui::PopStyleColor();
}

} // namespace Engine::Editor
