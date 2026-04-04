#include "pch.h"

#include "Core/App.h"

#include "Common.h"
#include "Renderer/Model.h"
#include "Scene/Object.h"

#include <utility>
#include <thread>

App::App()
    : window("Renderer", 1920, 1080)
{
}

void App::init()
{
    window.input = &input;
    
    appContext.pWindow = &window;
    appContext.pRenderer = &renderer;
    appContext.pScene = &scene;
    appContext.pInput = &input;

    init_scene();

    renderer.init();
    renderer.bind_scene(scene);

    for (auto& layer : layers)
    {
        layer->on_attach(appContext);
    }
}

i32 App::run()
{
    lastFpsTime = (f32)glfwGetTime();

    while (!window.should_close() && isRunning)
    {
        input.begin_frame();
        window.poll_events();

        f32 currentFrameTime = (f32)glfwGetTime();
        f32 deltaTime = currentFrameTime - lastFrame;
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

        for (auto& layer : layers)
            layer->on_update(appContext, deltaTime);

        scene.update();

        renderer.begin_frame();

        for (auto& layer : layers)
            layer->on_render(appContext);

        renderer.draw();
        renderer.end_frame();

        for (auto& layer : layers)
            layer->on_ui_render(appContext);

        window.swap_buffers();
    }

    for (auto it = layers.rbegin(); it != layers.rend(); ++it)
        (*it)->on_detach(appContext);

    return 0;
}

void App::init_scene()
{
    scene.load_from_file("Resources/Scenes/begin.ill");
}

void App::push_layer(std::unique_ptr<ILayer> layer)
{
    layers.push_back(std::move(layer));
}


void App::process_keyboard(f32 deltaTime)
{
    if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.window, true);

    if (glfwGetKey(window.window, GLFW_KEY_W) == GLFW_PRESS)
        scene.cameras[scene.currentCameraIdx].process_keyboard_forward(deltaTime);

    if (glfwGetKey(window.window, GLFW_KEY_S) == GLFW_PRESS)
        scene.cameras[scene.currentCameraIdx].process_keyboard_backward(deltaTime);

    if (glfwGetKey(window.window, GLFW_KEY_A) == GLFW_PRESS)
        scene.cameras[scene.currentCameraIdx].process_keyboard_left(deltaTime);

    if (glfwGetKey(window.window, GLFW_KEY_D) == GLFW_PRESS)
        scene.cameras[scene.currentCameraIdx].process_keyboard_right(deltaTime);

    if (glfwGetKey(window.window, GLFW_KEY_Q) == GLFW_PRESS)
        scene.cameras[scene.currentCameraIdx].process_keyboard_down(deltaTime);

    if (glfwGetKey(window.window, GLFW_KEY_E) == GLFW_PRESS)
        scene.cameras[scene.currentCameraIdx].process_keyboard_up(deltaTime);
}