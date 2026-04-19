#pragma once

#include <glad/glad.h>
#include <vector>
#include <numeric>

#include "Common.h"


struct Shader
{
    std::vector<GLuint> compiledShaders{};
    u32 shaderProgram = std::numeric_limits<u32>::max();

    enum class TYPE
    {
        FRAGMENT = 0x8B30,
        VERTEX   = 0x8B31,
        COUNT
    };

    void load_shader_from_source(const char* path, GLenum type);
    void create_and_link_program();
    i32  get_attrib_location(const char* name) const noexcept;
    i32  get_uniform_location(const char* name) const noexcept;
    void use() const;

private:

    static int _success;
    static char _log[512];

    void _error_check_shader_compilation(u32 shaderId) noexcept;
    void _error_check_shader_program_linkage(u32 programId) noexcept;
};