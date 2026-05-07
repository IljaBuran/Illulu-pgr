#pragma once

//-----------------------------------------------------------------------------------------
/**
 * \file       CollisionBoxComponent.h
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Defines an axis-aligned collision box component for simple object collision checks.
 */
 //-----------------------------------------------------------------------------------------

#include "Scene/IComponent.h"
#include "Common.h"

#include <glm/glm.hpp>
#include <utility>

struct CollisionBoxComponent : IComponent
{
    glm::vec3 offset;
    glm::vec3 size;

    CollisionBoxComponent(const glm::vec3& offset_ = {0.0f, 0.0f, 0.0f}, 
                          const glm::vec3& size_   = {1.0f, 1.0f, 1.0f})
        : offset(offset_), 
          size(size_) {}
    
    std::pair<glm::vec3, glm::vec3> get_min_max(const glm::vec3& objPos) const
    {
        glm::vec3 center = objPos + offset;
        glm::vec3 min = center - (size * 0.5f);
        glm::vec3 max = center + (size * 0.5f);
        return {min, max};
    }
    
    static bool collide(const CollisionBoxComponent& a, const glm::vec3& aPos, 
                        const CollisionBoxComponent& b, const glm::vec3& bPos)
    {
        const auto [aMin, aMax] = a.get_min_max(aPos);
        const auto [bMin, bMax] = b.get_min_max(bPos);

        return aMin.x <= bMax.x && aMax.x >= bMin.x &&
               aMin.y <= bMax.y && aMax.y >= bMin.y &&
               aMin.z <= bMax.z && aMax.z >= bMin.z;
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