#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       Window.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines a GLFW window wrapper with input handling and event callbacks.
 */
 //-----------------------------------------------------------------------------------------

struct GLFWwindow;

#include "Common.h"
#include "Core/Input.h"

struct Window
{
    GLFWwindow* window;
    Input* input = nullptr;
    i32 width;
    i32 height;

    Window() = delete;
    Window(const char* name, i32 width_, i32 height_) noexcept;
    ~Window();
    
    bool should_close() const noexcept;
    void poll_events() const noexcept;
    void swap_buffers() const noexcept;

private:

    static void _callback_close_window(GLFWwindow* window) noexcept;
    static void _callback_framebuffer_size(GLFWwindow* window, i32 width, i32 height) noexcept;
    static void _callback_set_mouse_cursor(GLFWwindow* window, f64 x, f64 y) noexcept;
    static void _callback_keyboard(GLFWwindow* window, i32 key, int scancode, int action, int mods) noexcept;
};