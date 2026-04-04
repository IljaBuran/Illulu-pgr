#pragma once

#include <string>

#include "glm/glm.hpp"
#include "Common.h"
#include "Scene/Transform.h"

using Color = glm::vec3;

struct Object
{
    std::string name = "unnamed";
    u32 modelIdx = -1;
    Transform transform;
    Color color{1.0f, 1.0f, 1.0f};
};