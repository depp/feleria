/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "quat.hpp"
#include <cmath>
namespace Base {

Quat Quat::rotation(Vec3 axis, float angle) {
    float a = 1.0f / axis.mag();
    float b = std::cos(0.5f * angle);
    float c = a * std::sin(0.5f * angle);
    return Quat { { b, c * axis[0], c * axis[1], c * axis[2] } };
}

Quat Quat::euler(Vec3 angles) {
    return (rotation(Vec3{0.0f, 0.0f, 1.0f}, angles[0]) *
            rotation(Vec3{0.0f, 1.0f, 0.0f}, angles[1]) *
            rotation(Vec3{1.0f, 0.0f, 0.0f}, angles[2]));
}

Vec3 Quat::transform(Vec3 p) const {
    float w = v[0], x = v[1], y = v[2], z = v[3];
    return Vec3 {
        p[0] * (1.0f - 2.0f*y*y - 2.0f*z*z) +
        p[1] * (2.0f*x*y - 2.0f*w*z) +
        p[2] * (2.0f*z*x + 2.0f*w*y),
        p[0] * (2.0f*x*y + 2.0f*w*z) +
        p[1] * (1.0f - 2.0f*z*z - 2.0f*x*x) +
        p[2] * (2.0f*y*z - 2.0f*w*x),
        p[0] * (2.0f*z*x - 2.0f*w*y) +
        p[1] * (2.0f*y*z + 2.0f*w*x) +
        p[2] * (1.0f - 2.0f*x*x - 2.0f*y*y)
    };
}

Quat Quat::conjugate( ) const {
    return Quat { { v[0], -v[1], -v[2], -v[3] } };
}

Quat operator*(Quat x, Quat y) {
    return Quat { {
        x[0]*y[0] - x[1]*y[1] - x[2]*y[2] - x[3]*y[3],
        x[0]*y[1] + x[1]*y[0] + x[2]*y[3] - x[3]*y[2],
        x[0]*y[2] + x[2]*y[0] + x[3]*y[1] - x[1]*y[3],
        x[0]*y[3] + x[3]*y[0] + x[1]*y[2] - x[2]*y[1]
    } };
}

Quat &operator*=(Quat &x, Quat y) {
    x = x * y;
    return x;
}

}
