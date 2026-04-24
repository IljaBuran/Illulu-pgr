#pragma once

#include "Scene/IComponent.h"
#include "Common.h"

#include <glm/glm.hpp>
#include <utility>

struct CollisionBoxComponent : IComponent
{
    glm::vec3 offset;
    glm::vec3 size;

    CollisionBoxComponent(const glm::vec3& offset_ = {0.0f, 0.0f, 0.0f}, const glm::vec3& size_ = {1.0f, 1.0f, 1.0f})
        : offset(offset_), size(size_) {}
    
    std::pair<glm::vec3, glm::vec3> get_min_max(const glm::vec3& objPos) const
    {
        glm::vec3 center = objPos + offset;
        glm::vec3 min = center - (size * 0.5f);
        glm::vec3 max = center + (size * 0.5f);
        return {min, max};
    }
    
    static bool collide(const CollisionBoxComponent& b1, const glm::vec3& b1_pos, 
                        const CollisionBoxComponent& b2, const glm::vec3& b2_pos)
    {
        const auto [b1_min, b1_max] = b1.get_min_max(b1_pos);
        const auto [b2_min, b2_max] = b2.get_min_max(b2_pos);

        return b1_min.x <= b2_max.x && b1_max.x >= b2_min.x &&
               b1_min.y <= b2_max.y && b1_max.y >= b2_min.y &&
               b1_min.z <= b2_max.z && b1_max.z >= b2_min.z;
    }


    ComponentKind get_component_kind() const override
    {
        return ComponentKind::CollisionBox;
    }
    
    std::unique_ptr<IComponent> clone() const override
    {
        return std::make_unique<CollisionBoxComponent>(*this);
    }
};