/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "mat.hpp"
namespace Base {

Vec3 Mat4::transform(Vec3 v) const {
    return Vec3 {
        v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + m[3][0],
        v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + m[3][1],
        v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + m[3][2]
    };
}

Mat4 Mat4::identity() {
    return Mat4 { {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f },
    } };
}

Mat4 Mat4::translation(Vec3 v) {
    return Mat4 { {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { v[0], v[1], v[2], 1.0f }
    } };
}

Mat4 Mat4::rotation(Quat q) {
    float w = q[0], x = q[1], y = q[2], z = q[3];
    return Mat4 { {
        { 1.0f - 2.0f*y*y - 2.0f*z*z,
          2.0f*x*y + 2.0f*w*z,
          2.0f*z*x - 2.0f*w*y,
          0.0f },
        { 2.0f*x*y - 2.0f*w*z,
          1.0f - 2.0f*z*z - 2.0f*x*x,
          2.0f*y*z + 2.0f*w*x,
          0.0f },
        { 2.0f*z*x + 2.0f*w*y,
          2.0f*y*z - 2.0f*w*x,
          1.0f - 2.0f*x*x - 2.0f*y*y,
          0.0f },
        { 0.0f,
          0.0f,
          0.0f,
          1.0f }
    } };
}

Mat4 Mat4::perspective(float mx, float my, float near, float far) {
    return Mat4 { {
        { 1.0f / mx, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f / my, 0.0f, 0.0f },
        { 0.0f, 0.0f, -(far + near) / (far - near), -1.0f },
        { 0.0f, 0.0f, -2.0f * far * near / (far - near), 0.0f }
    } };
}

Mat4 operator*(const Mat4 &x, const Mat4 &y) {
    Mat4 z;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            z.m[j][i] =
                x.m[0][i] * y.m[j][0] +
                x.m[1][i] * y.m[j][1] +
                x.m[2][i] * y.m[j][2] +
                x.m[3][i] * y.m[j][3];
        }
    }
    return z;
}

Mat4 &operator*=(Mat4 &x, const Mat4 &y) {
    x = x * y;
    return x;
}

}
