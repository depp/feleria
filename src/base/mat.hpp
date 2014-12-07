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
struct Mat3 {
    // m[column][row]
    float m[3][3];

    /// Get a pointer to the matrix elements.
    const float *data() const { return &m[0][0]; }
    /// Transform a (column) vector using the matrix.
    Vec3 transform(Vec3 v) const;
    /// Create the identity matrix.
    static Mat3 identity();
    /// Create a scale matrix.
    static Mat3 scale(float a);
    /// Create a scale matrix.
    static Mat3 scale(Vec3 v);
    /// Create a rotation matrix.
    static Mat3 rotation(Quat q);
};

Mat3 operator*(const Mat3 &x, const Mat3 &y);
Mat3 &operator*=(Mat3 &x, const Mat3 &y);

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
    /// Create a scale matrix.
    static Mat4 scale(float a);
    /// Create a scale matrix.
    static Mat4 scale(Vec3 v);
    /// Create a rotation matrix.
    static Mat4 rotation(Quat q);
    /// Create a perspective projection matrix.
    static Mat4 perspective(float mx, float my, float near, float far);
};

Mat4 operator*(const Mat4 &x, const Mat4 &y);
Mat4 &operator*=(Mat4 &x, const Mat4 &y);

}
#endif
