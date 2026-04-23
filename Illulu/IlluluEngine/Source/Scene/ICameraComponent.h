#pragma once
#include "Scene/IComponent.h"

#include "Common.h"

#include "glm/mat4x4.hpp"

struct Transform;

struct ICameraComponent : IComponent
{
    f32 yaw = -90.0f;
    f32 pitch = 0.0f;
    glm::vec3 worldUp{0.0f, 1.0f, 0.0f};
    
    ICameraComponent() = default;
    
    ICameraComponent(f32 yaw_, f32 pitch_, const glm::vec3& worldUp_)
        :  yaw(yaw_), pitch(pitch_), worldUp(worldUp_) {}

    virtual ~ICameraComponent() = default;

    glm::vec3 get_front() const
    {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        return glm::normalize(front);
    }
    
    glm::mat4 get_view_matrix(const Transform& transform) const
    {
        glm::vec3 front = get_front();
        glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
        glm::vec3 up = glm::normalize(glm::cross(right, front));

        return glm::lookAt(transform.position, transform.position + front, up);
    }

    virtual glm::mat4 get_projection_matrix(f32 aspect) const = 0;
};