#pragma once

#include <string>
#include <array>
#include "Common.h"

u32 load_texture_2d(const std::string& path);
u32 load_cubemap(const std::array<std::string, 6>& faces);