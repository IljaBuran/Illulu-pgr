#pragma once
#include "Common.h"
#include "Scene/IComponent.h"
#include <glm/glm.hpp>

using Color = glm::vec3;

struct ILightComponent : IComponent
{
	Color color;
	f32 intensity;

	ILightComponent(const Color& color_,
		                  f32    intensity_)
	    : color(color_), 
		  intensity(intensity_) {}
};