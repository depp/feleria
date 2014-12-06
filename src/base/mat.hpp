/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_MAT_HPP
#define LD_BASE_MAT_HPP
#include "vec.hpp"
#include "quat.hpp"
namespace Base {

/// Floating-point matrix.
struct Mat4 {
    // m[column][row]
    float m[4][4];

    /// Get a pointer to the matrix elements.
    const float *data() const { return &m[0][0]; }
    /// Transform a (column) vector using the matrix.
    Vec3 transform(Vec3 v) const;
    /// Create the identity matrix.
    static Mat4 identity();
    /// Create a translation matrix.
    static Mat4 translation(Vec3 v);
    /// Create a rotation matrix.
    static Mat4 rotation(Quat q);
    /// Create a perspective projection matrix.
    static Mat4 perspective(float mx, float my, float near, float far);
};

Mat4 operator*(const Mat4 &x, const Mat4 &y);
Mat4 &operator*=(Mat4 &x, const Mat4 &y);

}
#endif
