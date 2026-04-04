#pragma once

#include "Renderer/Vertex.h"

#include <vector>

struct Mesh
{
    u32 vao = 0;
    u32 vbo = 0;
    u32 ebo = 0;
    i32 indexCount = 0;

    u32 diffuseTexture = 0;

    Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices, u32 diffuseTexture);
    ~Mesh();

    Mesh(Mesh&& other)            noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    Mesh()                      = delete;
    Mesh(const Mesh&)           = delete;
    void operator=(const Mesh&) = delete;

    void bind() const;
    void draw() const;
    void destroy();
};