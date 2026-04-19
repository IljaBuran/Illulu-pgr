#pragma once

#include "IEditorPanel.h"

struct InspectorPanel : public IEditorPanel
{
    const char* get_name() const override { return "Inspector"; }
    void draw(AppContext& ctx, EditorState& state) override;
};