#include "pch.h"

#include <algorithm>
#include <utility>

#include "glm/glm.hpp"
#include "Core/App.h"

#include "Common.h"
#include "Core/Debug.h"

App::App()
    : window("IlluluEditor", 1920, 1080)
{
}

bool App::init()
{
    window.input = &input;

    appContext.pWindow = &window;
    appContext.pRenderer = &renderer;
    appContext.pScene = &scene;
    appContext.pInput = &input;
    appContext.lastFrameTime = &lastFpsTime;

    message_box::set_hwnd(static_cast<void*>(glfwGetWin32Window(window.window)));

    init_scene();

    renderer.init();
    renderer.bind_scene(scene);

    renderer.appContext = &appContext;

    lastFrame = (f32)glfwGetTime();
    lastFpsTime = lastFrame;
    deltaTime = 0.0f;
    fpsCnter = 0;
    lastFpsVal = 0;

    return true;
}

void App::shutdown()
{
}

void App::begin_frame()
{
    input.begin_frame();
    window.poll_events();

    f32 currentFrameTime = (f32)glfwGetTime();
    deltaTime = currentFrameTime - lastFrame;
    lastFrame = currentFrameTime;

    if (currentFrameTime - lastFpsTime < 1.0f)
    {
        fpsCnter++;
    }
    else
    {
        lastFpsTime = currentFrameTime;
        lastFpsVal = fpsCnter;
        fpsCnter = 0;
    }

    renderer.begin_frame();
}

void App::update()
{
    scene.update(deltaTime);
}

void App::render()
{
    renderer.draw();
}

void App::end_frame()
{
    renderer.end_frame();
    window.swap_buffers();
}

bool App::should_close() const
{
    return window.should_close() || !isRunning;
}

f32 App::get_delta_time() const
{
    return deltaTime;
}

AppContext& App::get_context()
{
    return appContext;
}

void App::close()
{
    isRunning = false;
}

void App::init_scene()
{
    // note(ilja): only here to not waste time by selecting which scene i want to open every time
    #if _DEBUG 
        scene.load_from_file("Resources/Scenes/semestral.ill");
    #else
        scene.load_from_file("Resources/Scenes/starter.ill");
    #endif
}