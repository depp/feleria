/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "shader.hpp"
namespace Graphics {
namespace Shader {
using Base::ShaderField;

#define AFIELD(name) { "in_" #name,       offsetof(TYPE, a_ ## name) }
#define UFIELD(name) {  "u_" #name,       offsetof(TYPE, u_ ## name) }
#define UARRAY(name) {  "u_" #name "[0]", offsetof(TYPE, u_ ## name) }

#define TYPE Text
const ShaderField TYPE::UNIFORMS[] = {
    UFIELD(vertxform),
    UFIELD(texscale),
    UFIELD(texture),
    UFIELD(color),
    { nullptr, 0 }
};

const ShaderField TYPE::ATTRIBUTES[] = {
    AFIELD(vert),
    { nullptr, 0 }
};
#undef TYPE

#define TYPE Sprite
const ShaderField TYPE::UNIFORMS[] = {
    UFIELD(modelview),
    UFIELD(projection),
    UFIELD(texscale),
    UFIELD(texture),
    { nullptr, 0 }
};

const ShaderField TYPE::ATTRIBUTES[] = {
    AFIELD(vert),
    AFIELD(texcoord),
    { nullptr, 0 }
};
#undef TYPE

#define TYPE World
const ShaderField TYPE::UNIFORMS[] = {
    UFIELD(modelview),
    UFIELD(projection),
    UFIELD(normalmat),
    UARRAY(terrain_color),
    UARRAY(light_dir),
    UARRAY(light_color),
    { nullptr, 0 }
};

const ShaderField TYPE::ATTRIBUTES[] = {
    AFIELD(vert),
    AFIELD(color),
    AFIELD(normal),
    { nullptr, 0 }
};
#undef TYPE

#define TYPE Ui
const ShaderField TYPE::UNIFORMS[] = {
    UFIELD(texture),
    { nullptr, 0 }
};

const ShaderField TYPE::ATTRIBUTES[] = {
    AFIELD(vert),
    { nullptr, 0 }
};
#undef TYPE

}
}
