#include "pch.h"
#include "Scene/PerspectiveCameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include "Scene/Transform.h"

PerspectiveCameraComponent::PerspectiveCameraComponent(f32 yaw_, f32 pitch_, const glm::vec3& worldUp_)
    : worldUp(worldUp_), yaw(yaw_), pitch(pitch_)
{}

glm::vec3 PerspectiveCameraComponent::get_front() const
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::normalize(front);
}

glm::mat4 PerspectiveCameraComponent::get_view_matrix(const Transform& transform) const
{
    glm::vec3 front = get_front();
    glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    return glm::lookAt(transform.position, transform.position + front, up);
}