#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       Vertex.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines a basic vertex structure with position, normal, and texture coordinates.
 */
 //-----------------------------------------------------------------------------------------

#include "glm/glm.hpp"

struct Vertex
{
    glm::vec3 position {0.0f, 0.0f, 0.0f};
    glm::vec3 normal   {0.0f, 0.0f, 1.0f};
    glm::vec2 uv       {0.0f, 0.0f};
};