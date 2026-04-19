#pragma once

#include "Scene/ILightComponent.h"

struct DirectionalLightComponent : ILightComponent
{
	DirectionalLightComponent(const Color& color_ = Color(1.0f), f32 intensity_ = 1.0f)
	{
		color = color_;
		intensity = intensity_;
	}

	ComponentKind get_component_kind() const override
	{
		return ComponentKind::DirectionalLight;
	}

	std::unique_ptr<IComponent> clone() const override
	{
		return std::make_unique<DirectionalLightComponent>(*this);
	}
};