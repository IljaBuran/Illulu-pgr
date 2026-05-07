//-----------------------------------------------------------------------------------------
/**
 * \file       pch.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines the editor precompiled header with common Windows, OpenGL, GLFW, ImGui, Native File Dialog, stb, and project includes.
 */
 //-----------------------------------------------------------------------------------------

#ifndef PCH_H
#define PCH_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <Windows.h>


// ==== OpenGL& GLFW ====
#include "glad/glad.h"
#include "GLFW/glfw3.h"

// ==== Imgui ====
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

// ==== NativeFileDialog ====
#include "nativefiledialog/nfd.hpp"
#include "nativefiledialog/nfd_glfw3.h"

// ==== stb ====
#include "stb/stb_image.h"

// ==== Common ====
#include "Common.h"
#include "Core/Debug.h"


#endif //PCH_H
