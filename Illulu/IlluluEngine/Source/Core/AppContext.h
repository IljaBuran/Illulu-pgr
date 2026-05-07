#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       AppContext.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines shared application context pointers used across engine systems (unsuccessfully).
 */
 //-----------------------------------------------------------------------------------------

#include "Common.h"

struct Window;
struct Renderer;
struct Scene;
struct Input;

struct AppContext
{
    Window*   pWindow   = nullptr;
    Renderer* pRenderer = nullptr;
    Scene*    pScene    = nullptr;
    Input*    pInput    = nullptr;

    f32* lastFrameTime  = nullptr;
};
