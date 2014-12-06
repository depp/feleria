/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_SHADER_HPP
#define LD_BASE_SHADER_HPP
#include "sg/opengl.h"
#include <cstddef>
#include <string>
namespace Base {

/// A field in an object which stores program attributes and uniform indexes.
struct ShaderField {
    /// The name of the uniform or attribute.
    const char *name;

    /// The offset of the corresponding GLint field in the structure.
    std::size_t offset;
};

/// Load an OpenGL shader program.  Returns 0 on failure.
GLuint load_program(const std::string &vertexshader,
                    const std::string &fragmentshader,
                    const ShaderField *uniforms,
                    const ShaderField *attributes,
                    void *object);

/// An OpenGL shader program.  The parameter T has uniforms and attributes.
template<class T>
class Program {
private:
    GLuint m_prog;
    T m_fields;

public:
    Program();
    Program(const Program &) = delete;
    Program(const Program &&other);
    ~Program();
    Program &operator=(const Program &) = delete;
    Program &operator=(Program &&other);

    /// Load the given shader program.
    bool load(const std::string &vertexshader,
              const std::string &fragmentshader);
    /// Get program attribute and uniform indexes.
    const T *operator->() const { return &m_fields; }
    /// Get the program object.
    GLuint prog() const { return m_prog; }
    /// Test whether the program is loaded.
    bool is_loaded() const { return m_prog != 0; }
};

template<class T>
Program<T>::Program() : m_prog(0) {}

template<class T>
Program<T>::Program(const Program &&other) {
    std::swap(m_prog, other.m_prog);
    m_fields = other.m_fields;
}

template<class T>
Program<T> &Program<T>::operator=(Program &&other) {
    std::swap(m_prog, other.m_prog);
    m_fields = other.m_fields;
    return *this;
}

template<class T>
bool Program<T>::load(const std::string &vertexshader,
                      const std::string &fragmentshader) {
    GLuint prog = load_program(
        vertexshader, fragmentshader,
        T::UNIFORMS, T::ATTRIBUTES,
        &m_fields);
    if (prog == 0) {
        return false;
    }
    glDeleteProgram(m_prog);
    m_prog = prog;
    return true;
}

template<class T>
Program<T>::~Program() {
    glDeleteProgram(m_prog);
}

}
#endif
