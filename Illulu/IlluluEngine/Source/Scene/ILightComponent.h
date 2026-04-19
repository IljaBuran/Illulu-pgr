#pragma once
#include "Common.h"
#include "Scene/IComponent.h"

using Color = glm::vec3;

struct ILightComponent : IComponent
{
	Color color{1.0f, 1.0f, 1.0f};
	f32 intensity = 1.0f;
};