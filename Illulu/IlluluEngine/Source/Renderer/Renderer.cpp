#include "pch.h"

#include "Renderer/Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene/Object.h"
#include "Scene/Scene.h"
#include "Renderer/Model.h"
#include "Renderer/Mesh.h"

#include "glad/glad.h"


void Renderer::init()
{
    shader.load_shader_from_source("Resources/Shaders/Basic.vert", GL_VERTEX_SHADER);
    shader.load_shader_from_source("Resources/Shaders/Basic.frag", GL_FRAGMENT_SHADER);
    shader.create_and_link_program();

    handles.PVM          = shader.get_uniform_location("PVM");
    handles.color        = shader.get_uniform_location("color");
    handles.texDiffuse   = shader.get_uniform_location("texDiffuse");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::begin_frame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.use();
    glUniform1i(handles.texDiffuse, 0);
}

void Renderer::bind_scene(const Scene& scene)
{
    pScene = &scene;
}

void Renderer::draw() const
{
    if (!pScene)
        FATAL_CRASH("[RENDERER]: No scene binded during Renderer::draw()!");

    for (const Object& obj : pScene->objects)
    {
        if (obj.modelIdx >= pScene->models.size())
        {
            FATAL_CRASH("[RENDERER]: Trying to renderer object with no model");
            return;
        }
        const Model& model = pScene->models[obj.modelIdx];

        glm::mat4 pvm = build_pvm(obj);

        glUniformMatrix4fv(handles.PVM, 1, GL_FALSE, glm::value_ptr(pvm));
        glUniform3fv(handles.color, 1, glm::value_ptr(obj.color));
        glActiveTexture(GL_TEXTURE0);

        for (const Mesh& mesh : model.meshes)
        {
            glBindTexture(GL_TEXTURE_2D, mesh.diffuseTexture);
            mesh.draw();
        }
    }
}

void Renderer::end_frame()
{
}

glm::mat4 Renderer::build_model_matrix(const Object& obj) const
{
    glm::mat4 model(1.0f);

    model = glm::translate(model, obj.transform.position);
    model = glm::rotate(model, glm::radians(obj.transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(obj.transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(obj.transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, obj.transform.scale);

    return model;
}

glm::mat4 Renderer::build_pvm(const Object& obj) const
{
    glm::mat4 model = build_model_matrix(obj);
    glm::mat4 view  = pScene->cameras[pScene->currentCameraIdx].get_view_matrix();
    return projection * view * model;
}