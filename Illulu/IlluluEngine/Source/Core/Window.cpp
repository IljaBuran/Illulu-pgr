#include "pch.h"

#include "Core/Window.h"
#include "Core/App.h"

Window::Window(const char* name, i32 width_, i32 height_) noexcept
    : width(width_), height(height_)
{       
    if (!glfwInit())
        FATAL_CRASH("GLFW failed to initialize!");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(width, height, name, NULL, NULL);
    if (!window)
        FATAL_CRASH("GLFW failed to create window!");

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1); // vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        FATAL_CRASH("GLAD failed to initialize!");

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(window, this);
    // callbacks
    glfwSetFramebufferSizeCallback(window, _callback_framebuffer_size);
    glfwSetKeyCallback(window, _callback_keyboard);
    glfwSetCursorPosCallback(window, _callback_mouse_cursor);
}

Window::~Window()
{
    glfwTerminate();
}

bool Window::should_close() const noexcept
{
    return glfwWindowShouldClose(window);
}

void Window::poll_events() const noexcept
{
    glfwPollEvents();
}

void Window::swap_buffers() const noexcept
{
    glfwSwapBuffers(window);
}



void Window::_callback_framebuffer_size([[maybe_unused]] GLFWwindow* window, i32 width_, i32 height_) noexcept
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->width = width_;
    self->height = height_;
    glViewport(0, 0, width_, height_);
}

void Window::_callback_keyboard(GLFWwindow* glfwWindow, i32 key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) noexcept
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!self || !self->input)
        return;

    if (action == GLFW_PRESS)
        self->input->set_key(key, true);
    else if (action == GLFW_RELEASE)
        self->input->set_key(key, false);
}

void Window::_callback_mouse_cursor(GLFWwindow* glfwWindow, double x, double y) noexcept
{
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    if (!self || !self->input)
        return;

    self->input->set_mouse_position(static_cast<f32>(x), static_cast<f32>(y));
}