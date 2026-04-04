#pragma once

#include "Common.h"
#include "IlluluEngine/Source/Core/ILayer.h"

struct App;

struct EditorLayer : public ILayer
{

    void on_attach(AppContext& ctx) override;
    void on_detach(AppContext& ctx) override;
    void on_update(AppContext& ctx, f32 deltaTime) override;
    void on_render(AppContext& ctx) override;
    void on_ui_render(AppContext& ctx) override;

	i32 selectedObject;
	i32 selectedCamera;

    bool editMode      = true;
    bool showImGuiDemo = false;
};

