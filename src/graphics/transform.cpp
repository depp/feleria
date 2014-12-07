/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "transform.hpp"
namespace Graphics {

Transform Transform::identity() {
    return Transform { Mat3::identity(), Mat4::identity() };
}

Transform Transform::translation(Vec3 v) {
    return Transform { Mat3::identity(), Mat4::translation(v) };
}

Transform Transform::scale(float a) {
    return Transform { Mat3::scale(1.0f / a), Mat4::scale(a) };
}

Transform Transform::scale(Vec3 v) {
    Vec3 i {{ 1.0f / v[0], 1.0f / v[1], 1.0f / v[2] }};
    return Transform { Mat3::scale(i), Mat4::scale(v) };
}

Transform Transform::rotation(Quat q) {
    return Transform { Mat3::rotation(q.conjugate()), Mat4::rotation(q) };
}

Transform operator*(const Transform &x, const Transform &y) {
    Transform t;
    t.normal = y.normal * x.normal;
    t.view = x.view * y.view;
    return t;
}

Transform &operator*=(Transform &x, const Transform &y) {
    x.normal = y.normal * x.normal;
    x.view = x.view * y.view;
    return x;
}

}

