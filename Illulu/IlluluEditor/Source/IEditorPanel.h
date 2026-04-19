#pragma once

#include "Core/AppContext.h"
#include "EditorState.h"

struct IEditorPanel
{
    virtual ~IEditorPanel() = default;
    virtual const char* get_name() const = 0;
    virtual void draw(AppContext& appCtx, EditorState& state) = 0;
};