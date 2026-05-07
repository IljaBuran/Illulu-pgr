//-----------------------------------------------------------------------------------------
/**
 * \file       pch.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines the precompiled header with common Windows, STL, GLM, OpenGL, GLFW, and project includes.
 */
 //-----------------------------------------------------------------------------------------

#ifndef PCH_H
#define PCH_H

// ==== Windows ====
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

// ==== STL ====

#include <vector>
#include <string>

// ==== GLM ====

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ==== OpenGL & GLFW ====

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// ==== Common ====
#include "Common.h"
#include "Core/Debug.h"

#endif // PCH_H
