/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_TRANSFORM_HPP
#define LD_GRAPHICS_TRANSFORM_HPP
#include "defs.hpp"
namespace Graphics {

struct Transform {
    Mat3 normal;
    Mat4 view;

    /// Create the identity transformation.
    static Transform identity();
    /// Create a translation transformation.
    static Transform translation(Vec3 v);
    /// Create a scale transformation.
    static Transform scale(float a);
    /// Create a scale transformation.
    static Transform scale(Vec3 v);
    /// Create a rotation transformation.
    static Transform rotation(Quat q);
};

Transform operator*(const Transform &x, const Transform &y);
Transform &operator*=(Transform &x, const Transform &y);

}
#endif
