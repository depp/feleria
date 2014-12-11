/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "file.hpp"
#include "shader.hpp"
#include "log.hpp"
#include "sg/entry.h"
#include "sg/shader.h"
#include <array>
#include <cstring>
#include <stdexcept>
namespace Base {

namespace {

struct LProgram {
    GLuint program;
    std::string name;

    static LProgram none() {
        return LProgram { 0, std::string() };
    }
};

GLint &get_field(const ShaderField &field, void *object) {
    return *reinterpret_cast<GLint *>(
        static_cast<char *>(object) + field.offset);
}

/// Load the indexes of shader uniforms into an object.
void get_uniforms(const LProgram &program, void *object,
                  const ShaderField *uniforms) {
    for (int i = 0; uniforms[i].name; i++) {
        GLint value = glGetUniformLocation(program.program, uniforms[i].name);
        if (value < 0) {
            Log::warn("%s: Uniform does not exist: %s",
                      program.name.c_str(), uniforms[i].name);
        }
        get_field(uniforms[i], object) = value;
    }
    GLint ucount = 0;
    glGetProgramiv(program.program, GL_ACTIVE_UNIFORMS, &ucount);
    for (int j = 0; j < ucount; j++) {
        char name[32];
        name[0] = '\0';
        GLint size;
        GLenum type;
        glGetActiveUniform(program.program, j, sizeof(name), nullptr,
                           &size, &type, name);
        for (int i = 0; ; i++) {
            if (!uniforms[i].name) {
                Log::warn("%s: Unbound uniform: %s",
                          program.name.c_str(), name);
                break;
            }
            if (!std::strcmp(name, uniforms[i].name)) {
                break;
            }
        }
    }
}

/// Load the indexes of shader attributes into an object.
void get_attributes(const LProgram &program, void *object,
                    const ShaderField *attributes) {
    for (int i = 0; attributes[i].name; i++) {
        GLint *ptr = reinterpret_cast<GLint *>(
            static_cast<char *>(object) + attributes[i].offset);
        *ptr = glGetAttribLocation(program.program, attributes[i].name);
    }
}

struct ShaderFile {
    const std::string &path;
    GLenum type;
};

template<class I>
LProgram load_program2(I first, I last) {
    GLenum program;
    program = glCreateProgram();
    if (!program)
        return LProgram::none();
    std::string name;
    for (I i = first; i != last; ++i) {
        if (!name.empty())
            name += ',';
        name += i->path;
        std::string path = shader_path;
        path += '/';
        path += i->path;
        GLuint shader = sg_shader_file(
            path.data(), path.size(), i->type, nullptr);
        if (!shader) {
            glDeleteProgram(program);
            return LProgram::none();
        }
        glAttachShader(program, shader);
        glDeleteShader(shader);
    }
    int r = sg_shader_link(program, name.c_str(), nullptr);
    if (r) {
        glDeleteProgram(program);
        return LProgram::none();
    }
    return LProgram { program, std::move(name) };
}

}

std::string shader_path("shader");

GLuint load_program(const std::string &vertexshader,
                    const std::string &fragmentshader,
                    const ShaderField *uniforms,
                    const ShaderField *attributes,
                    void *object) {
    std::array<ShaderFile, 2> files { {
        { vertexshader, GL_VERTEX_SHADER },
        { fragmentshader, GL_FRAGMENT_SHADER }
    } };
    LProgram program = load_program2(std::begin(files), std::end(files));
    if (program.program) {
        get_uniforms(program, object, uniforms);
        get_attributes(program, object, attributes);
    }
    return program.program;
}

}
