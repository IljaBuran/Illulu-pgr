#pragma once

#include <algorithm>

#include "Common.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct Camera
{
    std::string name = "camera";
    glm::vec3 position{0.0f, 0.0f, 3.0f};
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    glm::vec3 right{1.0f, 0.0f, 0.0f};
    glm::vec3 worldUp{0.0f, 1.0f, 0.0f};

    f32 yaw   = -90.0f; // left right
    f32 pitch = 0.0f; // up down

    static constexpr f32 movementSpeed    = 9.0f;
    static constexpr f32 mouseSensitivity = 0.1f;

    glm::mat4 get_view_matrix() const
    {
        return glm::lookAt(position, position + front, up);
    }

    void update()
    {
        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(f);

        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }

    void process_keyboard_forward(f32 deltaTime)
    {
        position += front * movementSpeed * deltaTime;
    }

    void process_keyboard_backward(f32 deltaTime)
    {
        position -= front * movementSpeed * deltaTime;
    }

    void process_keyboard_left(f32 deltaTime)
    {
        position -= right * movementSpeed * deltaTime;
    }

    void process_keyboard_right(f32 deltaTime)
    {
        position += right * movementSpeed * deltaTime;
    }

    void process_keyboard_up(f32 deltaTime)
    {
        position += worldUp * movementSpeed * deltaTime;
    }

    void process_keyboard_down(f32 deltaTime)
    {
        position -= worldUp * movementSpeed * deltaTime;
    }

    void process_mouse_movement(f32 xOffset, f32 yOffset)
    {
        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;
        yaw   += xOffset;
        pitch += yOffset;

        pitch = std::clamp(pitch, -89.0f, 89.0f);
        //if (pitch > 89.0f)  pitch = 89.0f;
        //if (pitch < -89.0f) pitch = -89.0f;

        update();
    }
};