#include "pch.h"

#include "Common.h"


#include "Renderer/Mesh.h"

#include <cstddef>
#include <glad/glad.h>

Mesh::Mesh(const std::vector<Vertex>& vertices_, const std::vector<u32>& indices_, const Material& material_)
    : material(material_)
{
    indexCount = (i32)indices_.size();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices_.size() * sizeof(Vertex)),
        vertices_.data(),
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices_.size() * sizeof(u32)),
        indices_.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, position))
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, normal))
    );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, uv))
    );

    glBindVertexArray(0);
}

Mesh::Mesh(Mesh&& other) noexcept
{
        vao = other.vao;
        vbo = other.vbo;
        ebo = other.ebo;
        indexCount = other.indexCount;
        material = other.material;

        other.vao = 0;
        other.vbo = 0;
        other.ebo = 0;
        other.indexCount = 0;
        other.material = {};
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        destroy();

        vao = other.vao;
        vbo = other.vbo;
        ebo = other.ebo;
        indexCount = other.indexCount;
        material = other.material;

        other.vao = 0;
        other.vbo = 0;
        other.ebo = 0;
        other.indexCount = 0;
        other.material = {};
    }

    return *this;
}

Mesh::~Mesh()
{
    destroy();
}

void Mesh::bind() const
{
    glBindVertexArray(vao);
}

void Mesh::draw() const
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::destroy()
{
    if (vao) 
        glDeleteVertexArrays(1, &vao);
    if (vbo) 
        glDeleteBuffers(1, &vbo);
    if (ebo) 
        glDeleteBuffers(1, &ebo);

    vao = 0;
    vbo = 0;
    ebo = 0;
    indexCount = 0;
    material = {};
}