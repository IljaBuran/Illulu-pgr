#pragma once
#include "Scene/IComponent.h"

#include "Common.h"

#include <glm/glm.hpp>

struct Transform;

struct ICameraComponent : IComponent
{
    f32 yaw;
    f32 pitch;
    glm::vec3 worldUp;
    
    ICameraComponent() = default;
    
    ICameraComponent(      f32        yaw_     = -90.0f, 
                           f32        pitch_   = 0.0f, 
                     const glm::vec3& worldUp_ = glm::vec3{0.0f, 1.0f, 0.0f})
        :  yaw(yaw_), 
           pitch(pitch_), 
           worldUp(worldUp_) {}

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