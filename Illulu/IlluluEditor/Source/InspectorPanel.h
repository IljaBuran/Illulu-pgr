//-----------------------------------------------------------------------------------------
/**
 * \file       InspectorPanel.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines the editor inspector panel for viewing and editing selected object properties.
 */
 //-----------------------------------------------------------------------------------------

#pragma once

#include "IEditorPanel.h"

struct InspectorPanel : public IEditorPanel
{
    const char* get_name() const override { return "Inspector"; }
    void draw(AppContext& ctx, EditorState& state) override;
};