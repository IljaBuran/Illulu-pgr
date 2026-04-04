#pragma once

#include <memory>

#include "Common.h"
#include "Core/Window.h"
#include "Core/AppContext.h"
#include "Renderer/Renderer.h"
#include "Scene/Scene.h"
#include "Core/ILayer.h"

struct ILayer;

struct Window;
struct Renderer;
struct Scene;
struct Input;

struct App
{
    App();

    void init();
    void init_scene();

    i32 run();
    
    void push_layer(std::unique_ptr<ILayer> layer);

    void process_keyboard(f32 deltaTime);
    void process_mouse();

    Window     window;
    Renderer   renderer;
    Scene      scene;
    Input      input;
    AppContext appContext;
    
    std::vector<std::unique_ptr<ILayer>> layers;

    bool isRunning = true;

    f32 lastFrame = 0.0f;
    u16 fpsCnter    = 0;
    f32 lastFpsTime = 0;
    u16 lastFpsVal  = 0;
};