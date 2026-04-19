#pragma once

#include "IEditorPanel.h"

struct InfoPanel : public IEditorPanel
{
    const char* get_name() const override { return "Info"; }
    void draw(AppContext& ctx, EditorState& state) override;
};