#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       InfoPanel.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines the editor information panel for displaying scene and application status.
 */
 //-----------------------------------------------------------------------------------------

#include "IEditorPanel.h"

struct InfoPanel : public IEditorPanel
{
    const char* get_name() const override { return "Info"; }
    void draw(AppContext& ctx, EditorState& state) override;
};