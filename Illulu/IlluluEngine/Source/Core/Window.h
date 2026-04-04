#pragma once

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

struct GLFWwindow;

#include "Common.h"
#include "Core/Input.h"

struct Window
{
    GLFWwindow* window;
    Input* input = nullptr;

    Window() = delete;
    Window(const char* name, i32 width, i32 height) noexcept;
    ~Window();
    
    bool should_close() const noexcept;
    void poll_events() const noexcept;
    void swap_buffers() const noexcept;

    static void message_box(const char* title, const char* body);

private:

    static void _callback_framebuffer_size(GLFWwindow* window, i32 width, i32 height) noexcept;
    static void _callback_mouse_cursor(GLFWwindow* window, f64 x, f64 y) noexcept;
    static void _callback_keyboard(GLFWwindow* window, i32 key, int scancode, int action, int mods) noexcept;
};