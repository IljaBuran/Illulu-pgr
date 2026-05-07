//-----------------------------------------------------------------------------------------
/**
 * \file       MenuBarPanel.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines the editor menu bar panel interface.
 */
 //-----------------------------------------------------------------------------------------

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

