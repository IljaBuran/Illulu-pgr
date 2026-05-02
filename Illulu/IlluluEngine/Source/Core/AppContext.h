#pragma once
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
