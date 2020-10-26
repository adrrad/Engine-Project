#pragma once
#include "core/Scene.hpp"
#include "assets/AssetManager.hpp"
#include "platform/WindowManager.hpp"
#include "platform/io/Directory.hpp"

#include "editor/gui/Panel.hpp"
#include "editor/gui/FilesPanel.hpp"
#include "editor/gui/SelectableItem.hpp"
#include <functional>

namespace Engine::Editor
{

class SceneInspector
{
private:
    Assets::AssetManager* m_assetManager = nullptr;
    Platform::WindowManager* m_windowManager = nullptr;
    Core::Scene* m_scene = nullptr;
    Core::GameObject* m_draggedGO = nullptr;
    SelectableItem* m_selectedItem = nullptr;

    Panel topPanel = Panel("Controls", 0, 0, 0, 0,
        PanelPlacement::AUTO, PanelAlignment::TOPLEFT);

    Panel leftPanel = Panel("Overview", 0, 0, 0, GUIProperties::WindowHeight,
        PanelPlacement::AUTO, PanelAlignment::TOPLEFT);

    Panel rightPanel = Panel("Inspector", 0, 0, 0, GUIProperties::WindowHeight,
        PanelPlacement::AUTO, PanelAlignment::RIGHT);

    FilesPanel filesPanel;

    std::function<void()> playCallback, pauseCallback, stopCallback;

    void SetAssetPayload(Engine::Platform::IO::File* assetFile);

    void DrawGameObjectNode(Engine::Core::GameObject* gameObject);

    void DrawSceneGraph();

    void DrawFileInspector();

    void DrawGameObjectInspector();

    void DrawControlPanel();

    // void DrawDirectoryContent(Platform::IO::Directory* dir);

    // void DrawProjectFiles();

    void DrawRightPanel();

    void DrawLeftPanel();

    void DrawTopPanel();

    void DrawBottomPanel();

    template <class C>
    void SelectItem(C* item);

    template <class C>
    C* GetSelectedItem();

    void LoadFonts();

public:
    SceneInspector();

    void SetScene(Core::Scene* scene);

    void DrawGUI();

    void SetPlayCallback(std::function<void()> callback);

    void SetPauseCallback(std::function<void()> callback);
    
    void SetStopCallback(std::function<void()> callback);
}; 


template <class C>
void SceneInspector::SelectItem(C* item)
{
    if(m_selectedItem) delete m_selectedItem;
    m_selectedItem = new SelectedItem(item);
}

template <class C>
C* SceneInspector::GetSelectedItem()
{
    if(m_selectedItem)
    {
        auto s = dynamic_cast<SelectedItem<C>*>(m_selectedItem);
        if(s != nullptr) return s->Value;
    }
    return nullptr;
}

} // namespace GUI
