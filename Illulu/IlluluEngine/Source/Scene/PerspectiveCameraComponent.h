#pragma once

#include "Scene/ICameraComponent.h"
#include "glm/glm.hpp"

struct PerspectiveCameraComponent : ICameraComponent
{
    glm::vec3 worldUp{0.0f, 1.0f, 0.0f};
    f32 yaw = -90.0f;
    f32 pitch = 0.0f;

    PerspectiveCameraComponent() = default;
    PerspectiveCameraComponent(f32 yaw_, f32 pitch_, const glm::vec3& worldUp_ = glm::vec3(0.0f, 1.0f, 0.0f));

    ComponentKind get_component_kind() const override
    {
        return ComponentKind::PerspectiveCamera;
    }

    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<PerspectiveCameraComponent>(*this);
    }

    glm::mat4 get_view_matrix(const Transform& transform) const override;
    glm::vec3 get_front() const;
};
