#include "pch.h"

#include "Core/Filesystem.h"
#include "Renderer/Shader.h"

#include "glad/glad.h"

#include <string>
// todo
#include <iostream>

int Shader::_success = 0;
char Shader::_log[512] = {};

void Shader::load_shader_from_source(const char *path, GLenum type)
{
    std::string shaderSrc = read_file_to_string(path);
    const char* shaderSrcRaw = shaderSrc.c_str();

    u32 shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &shaderSrcRaw, NULL);

    glCompileShader(shaderId);

    _error_check_shader_compilation(shaderId);

    compiledShaders.emplace_back(shaderId);
}

void Shader::create_and_link_program()
{
    shaderProgram = glCreateProgram();
    for (u32 shaderId : compiledShaders)
    {
        glAttachShader(shaderProgram, shaderId);
    }
    glLinkProgram(shaderProgram);

    _error_check_shader_program_linkage(shaderProgram);

    for (u32 shaderId : compiledShaders)
    {
        glDeleteShader(shaderId);
    }
    compiledShaders.clear();
}

i32 Shader::get_attrib_location(const char* name) const noexcept
{
    return glGetAttribLocation(shaderProgram, name);
}

i32 Shader::get_uniform_location(const char* name) const noexcept
{
    return glGetUniformLocation(shaderProgram, name);
}

void Shader::use() const
{
    glUseProgram(shaderProgram);
}

void Shader::_error_check_shader_compilation(u32 shaderId) noexcept
{
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &_success);

    if (!_success)
    {
        glGetShaderInfoLog(shaderId, sizeof(_log), NULL, _log);
        std::cout << "Shader compile error:\n" << _log << '\n';
        FATAL_CRASH(_log);
    }
}

void Shader::_error_check_shader_program_linkage(u32 programId) noexcept
{
    glGetProgramiv(programId, GL_LINK_STATUS, &_success);

    if (!_success)
    {
        glGetProgramInfoLog(programId, sizeof(_log), NULL, _log);
        std::cout << "Program link error:\n" << _log << '\n';
        FATAL_CRASH(_log);
    }
}