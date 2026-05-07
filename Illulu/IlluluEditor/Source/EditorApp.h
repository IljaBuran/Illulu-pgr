#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       EditorApp.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines the editor application wrapper that connects the engine with editor UI panels.
 */
 //-----------------------------------------------------------------------------------------

#include <memory>

#include "Common.h"
#include "Core/App.h"

struct EditorState;
struct MenuBarPanel;
struct HierarchyPanel;
struct InspectorPanel;
struct InfoPanel;
struct AssetPanel;

struct EditorApp
{
    EditorApp();
    ~EditorApp();

    bool init();
    i32 run();
    void shutdown();

private:
    void update(f32 deltaTime);
    void render();
    void ui_render();

    void begin_imgui_frame();
    void end_imgui_frame();

private:
    App engine;

    std::unique_ptr<EditorState>    state;
    std::unique_ptr<MenuBarPanel>   menuBarPanel;
    std::unique_ptr<HierarchyPanel> hierarchyPanel;
    std::unique_ptr<InspectorPanel> inspectorPanel;
    std::unique_ptr<InfoPanel>      infoPanel;
    std::unique_ptr<AssetPanel>     assetPanel;
};