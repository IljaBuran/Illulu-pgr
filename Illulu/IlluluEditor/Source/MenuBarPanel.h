#pragma once

#include "IEditorPanel.h"

struct MenuBarPanel : public IEditorPanel
{
    const char* get_name() const override
    {
        return "MenuBar";
    }

    void draw(AppContext& appCtx, EditorState& state) override;
};

