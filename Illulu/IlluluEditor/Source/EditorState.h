#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       EditorState.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines shared editor UI state such as selected objects, modes, and debug view flags.
 */
 //-----------------------------------------------------------------------------------------

#include "Common.h"

struct EditorState
{
    i32 selectedObject = -1;
    i32 selectedModel = -1;
    bool editMode = true;
    bool showImGuiDemo = false;
    bool locked = false;

    // views
    bool showCollisionBoxes = false;
};

