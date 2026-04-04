#pragma once

#include "glm/mat4x4.hpp"
#include "Renderer/Shader.h"
#include "Scene/Scene.h"
#include "Scene/Object.h"

struct Model;

struct RendererHandles
{
    i32 PVM        = -1;
    i32 color      = -1;
    i32 texDiffuse = -1;
};

struct Renderer
{
    Shader shader;
    RendererHandles handles;

    void init();
    void begin_frame();
    void bind_scene(const Scene& scene);
    void draw() const;
    void end_frame();

private:
    const Scene* pScene = nullptr;

    glm::mat4 build_model_matrix(const Object& obj) const;
    glm::mat4 build_pvm(const Object& obj) const;

    glm::mat4 projection = glm::perspective(glm::radians(75.0f), 1920.0f / 1080.0f, 0.1f, 180.0f);
};