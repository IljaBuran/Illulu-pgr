#pragma once

#include "IEditorPanel.h"

struct HierarchyPanel : public IEditorPanel
{
    const char* get_name() const override 
    { 
        return "Hierarchy"; 
    }
    void draw(AppContext& ctx, EditorState& state) override;

private:
    void clamp_selection(struct Scene& scene, EditorState& state);
};

