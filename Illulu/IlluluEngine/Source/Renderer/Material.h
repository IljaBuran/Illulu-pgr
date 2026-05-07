//-----------------------------------------------------------------------------------------
/**
 * \file       Material.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines material properties used for textured and lit mesh rendering.
 */
 //-----------------------------------------------------------------------------------------

#pragma once
#include "Common.h"
#include <glm/glm.hpp>

enum class BlendMode : u8
{
    Opaque,
    Masked,
    Transparent
};

struct Material
{
    u32 diffuseTex = 0;
    u32 specularTex = 0;

    glm::vec4 diffuseCol  = glm::vec4(1.0f);
    glm::vec3 specularCol = glm::vec3(0.25f);

    f32 shininess = 32.0f;
    BlendMode blendMode = BlendMode::Opaque;
};