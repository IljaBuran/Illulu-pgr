#include "pch.h"
#include "Object.h"

Object::Object(const Object& other)
{
    name = other.name + "_copy";
    transform = other.transform;

    components.reserve(other.components.size());

    for (const auto& comp : other.components)
        components.push_back(comp->clone());
}

Object& Object::operator=(const Object& other)
{
    if (this == &other)
        return *this;

    components.clear();

    name = other.name + "_copy";
    transform = other.transform;

    components.reserve(other.components.size());

    for (const auto& comp : other.components)
        components.push_back(comp->clone());

    return *this;
}
