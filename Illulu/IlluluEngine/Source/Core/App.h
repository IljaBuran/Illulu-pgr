#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       App.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines the main application class that owns core systems and controls the frame loop.
 */
 //-----------------------------------------------------------------------------------------

#include "Common.h"
#include "Core/AppContext.h"
#include "Core/Window.h"
#include "Core/Input.h"
#include "Renderer/Renderer.h"
#include "Scene/Scene.h"

struct App
{
    App();

    bool init();
    void shutdown();

    void init_scene();

    void begin_frame();
    void update();
    void render();
    void end_frame();

    bool should_close() const;
    f32 get_delta_time() const;
    AppContext& get_context();

    void close();

    Window     window;
    Renderer   renderer;
    Scene      scene;
    Input      input;
    AppContext appContext;

    bool isRunning = true;

    f32 lastFrame = 0.0f;
    f32 deltaTime = 0.0f;
    u16 fpsCnter = 0;
    f32 lastFpsTime = 0.0f;
    u16 lastFpsVal = 0;

    bool sceneNotBound = true;
};