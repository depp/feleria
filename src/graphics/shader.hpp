/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_SHADER_HPP
#define LD_GRAPHICS_SHADER_HPP
#include "base/shader.hpp"
namespace Graphics {
namespace Shader {

/// Uniforms and attributes for the "text" shader.
struct Text {
    static const Base::ShaderField UNIFORMS[];
    static const Base::ShaderField ATTRIBUTES[];

    // Attributes
    GLint a_vert;

    // Uniforms
    GLint u_vertxform;
    GLint u_texscale;
    GLint u_texture;
    GLint u_color;
};

/// Uniforms and attributes for the "sprite" shader.
struct Sprite {
    static const Base::ShaderField UNIFORMS[];
    static const Base::ShaderField ATTRIBUTES[];

    // Attributes
    GLint a_vert;
    GLint a_texcoord;

    // Uniforms
    GLint u_vertxform;
    GLint u_texscale;
    GLint u_texture;
    GLint u_color;
};

}
}
#endif
