#pragma once
#include "Common.h"

struct EditorState
{
    i32 selectedObject = -1;
    i32 selectedModel = -1;
    bool editMode = true;
    bool showImGuiDemo = false;
    bool locked = false;
};

