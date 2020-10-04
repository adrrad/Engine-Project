#pragma once

#include "editor/gui/Container.hpp"
#include "editor/gui/GUIProperties.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#include <string>

namespace Engine::Editor
{ 

enum class PanelAlignment { TOPLEFT = 0, BOTTOM = 1, RIGHT = 2 };
enum class PanelPlacement { FIXED, AUTO };

inline PanelAlignment operator|(PanelAlignment a, PanelAlignment b)
{
    return static_cast<PanelAlignment>(static_cast<int>(a) | static_cast<int>(b));
}

inline PanelAlignment operator&(PanelAlignment a, PanelAlignment b)
{
    return static_cast<PanelAlignment>(static_cast<int>(a) & static_cast<int>(b));
}

class Panel : public Container
{
public:
    std::string Name;
    bool Locked = false;
    PanelAlignment Alignment = PanelAlignment::TOPLEFT;
    PanelPlacement Placement = PanelPlacement::FIXED;
    inline Panel(std::string name = "Panel") 
    {
        Name = name;
    };

    inline Panel(std::string name, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        Name = name;
        X = x; Y = y;
        Width = width;
        Height = height;
        MinWidth = 0; MinHeight = 0;
        MaxWidth = 0; MaxHeight = 0;
    }

    inline Panel(std::string name, uint32_t x, uint32_t y, 
        uint32_t width, uint32_t height, uint32_t minWidth, 
        uint32_t minHeight, uint32_t maxWidth, uint32_t maxHeight)
        : Panel(name, x, y, width, height)
    {
        MinWidth = minWidth;
        MinHeight = minHeight;
        MaxWidth = maxWidth;
        MaxHeight = maxHeight;
    }

    inline Panel(std::string name, uint32_t x, uint32_t y, 
        uint32_t width, uint32_t height, 
        PanelPlacement placement, PanelAlignment alignment) 
        : Panel(name, x, y, width, height)
    {
        Placement = placement;
        Alignment = alignment;
    }


    inline void Begin()
    {
        ImGui::PushID(this);
        ImGui::SetNextWindowPos(GetWindowPosition());

        ImGui::SetNextWindowSizeConstraints(ImVec2(MinWidth, MinHeight), GetMaxSize());
        ImGui::Begin(Name.c_str(), (bool*)0, Locked ? ImGuiWindowFlags_NoMove : 0);
        auto [w, h] = ImGui::GetWindowSize();
        Width = w;
        Height = h;
    }

    inline void End()
    {
        ImGui::End();
        ImGui::PopID();
    }

private:

    inline ImVec2 GetMaxSize()
    {
        float w = MaxWidth == 0 ? Width : MaxWidth;
        float h = MaxHeight == 0 ? Height : MaxHeight;
        return ImVec2(w,h);
    }

    inline ImVec2 GetWindowPosition()
    {
        float x = float(X), y = float(Y);
        if(Placement == PanelPlacement::FIXED) return ImVec2(x, y);
        if(int(Alignment & PanelAlignment::TOPLEFT)) return ImVec2(0, 0);
        if(int(Alignment & PanelAlignment::RIGHT)) x =  float(GUIProperties::WindowWidth) - Width;
        if(int(Alignment & PanelAlignment::BOTTOM)) y = float(GUIProperties::WindowHeight) - Height;
        return ImVec2(x, y);
    }

};

} // namespace Engine::Editor
