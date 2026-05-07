#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       Texture.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Declares helper functions for loading 2D textures and cubemaps.
 */
 //-----------------------------------------------------------------------------------------

#include <string>
#include <array>
#include "Common.h"

u32 load_texture_2d(const std::string& path);
u32 load_cubemap(const std::array<std::string, 6>& faces);