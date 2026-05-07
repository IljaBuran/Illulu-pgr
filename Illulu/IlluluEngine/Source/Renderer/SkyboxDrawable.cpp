//-----------------------------------------------------------------------------------------
/**
 * \file       SkyboxDrawable.cpp
 * \author     Ilja Buran
 * \date       2025/05/07
 * \brief      Implements cubemap loading, skybox drawing, and skybox OpenGL resource management.
 */
 //-----------------------------------------------------------------------------------------

#include "pch.h"
#include "Renderer/SkyboxDrawable.h"

#include <glad/glad.h>

#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"

SkyboxDrawable::SkyboxDrawable(const std::array<std::string, 6>& faces_)
    : faces(faces_)
{
    cubemapTexture = load_cubemap(faces);
    init_cube();
}

SkyboxDrawable::~SkyboxDrawable()
{
    destroy();
}

void SkyboxDrawable::draw(Renderer& renderer, [[maybe_unused]] const Transform& transform) const
{
    renderer.draw_skybox(cubemapTexture, vao);
}

void SkyboxDrawable::init_cube()
{
    static const f32 vertices[] =
    {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);

    glBindVertexArray(0);
}

void SkyboxDrawable::destroy()
{
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (cubemapTexture) glDeleteTextures(1, &cubemapTexture);

    vao = 0;
    vbo = 0;
    cubemapTexture = 0;
}