#pragma once
#include "Common.h"
#include <numeric>
#include <memory>
#include <string>
#include "Core/Debug.h"
#include "Scene/IComponent.h"
#include <limits>

struct ComponentRef
{
    u32 objectIdx    = std::numeric_limits<u32>::max();
    u32 componentIdx = std::numeric_limits<u32>::max();

    bool operator==(const ComponentRef&) const = default;
};

enum class ComponentKind : i8
{
    DrawableModel,
    DrawableSkybox,
    PerspectiveCamera,
    OrthographicCamera,
    FreeCameraController,
    DirectionalLight,
    PointLight,
    SpotLight,
    COUNT
};

struct IComponent
{
    virtual ~IComponent() = default;

    virtual ComponentKind get_component_kind() const = 0;
    
    virtual std::unique_ptr<IComponent> clone() const = 0;
};

inline const char* component_kind_to_string(ComponentKind ck)
{
    if (ck >= ComponentKind::COUNT)
        FATAL_CRASH("[ComponentKind]: Unknown kind");

    if (ck == ComponentKind::DrawableModel)
        return "DrawableModel";
    else if (ck == ComponentKind::DrawableSkybox)
        return "DrawableSkybox";
    else if (ck == ComponentKind::PerspectiveCamera)
        return "PerspectiveCamera";
    else if (ck == ComponentKind::OrthographicCamera)
        return "OrthographicCamera";
    else if (ck == ComponentKind::FreeCameraController)
        return "FreeCameraController";
    else if (ck == ComponentKind::DirectionalLight)
        return "DirectionalLight";
    else if (ck == ComponentKind::PointLight)
        return "PointLight";
    else if (ck == ComponentKind::SpotLight)
        return "SpotLight";

    return "Here only for the compiler bitching, this should not happen otherwise it's gg";
}

inline ComponentKind component_kind_from_string(const std::string& s)
{
    if (s == "DrawableModel")
        return ComponentKind::DrawableModel;
    else if (s == "DrawableSkybox")
        return ComponentKind::DrawableSkybox;
    else if (s == "PerspectiveCamera")
        return ComponentKind::PerspectiveCamera;
    else if (s == "OrthographicCamera")
        return ComponentKind::OrthographicCamera;
    else if (s == "FreeCameraController")
        return ComponentKind::FreeCameraController;
    else if (s == "DirectionalLight")
        return ComponentKind::DirectionalLight;
    else if (s == "PointLight")
        return ComponentKind::PointLight;
    else if (s == "SpotLight")
        return ComponentKind::SpotLight;
    else
        FATAL_CRASH("[ComponentKind]: Unknown kind");   
}