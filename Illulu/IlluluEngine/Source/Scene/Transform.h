#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       Transform.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines a basic transform structure for storing position, rotation, and scale.
 */
 //-----------------------------------------------------------------------------------------

#include "glm/glm.hpp"
struct Transform
{
    glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
    glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
};