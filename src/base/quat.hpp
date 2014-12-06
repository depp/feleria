/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_QUAT_HPP
#define LD_BASE_QUAT_HPP
#include "vec.hpp"
namespace Base {

/// Quaternion.
struct Quat {
    /// Real component, then i, j, k components.
    float v[4];

    float operator[](int i) const { return v[i]; }
    float &operator[](int i) { return v[i]; }

    /// Unit quaternion.
    static Quat one();
    /// Create a rotor.
    static Quat rotation(Vec3 axis, float angle);
    /// Create a rotor from Euler angles.
    static Quat euler(Vec3 angles);
    /// Transform a vector using the quaternion.
    Vec3 transform(Vec3 p) const;
    /// Get the quaternion's conjugate.
    Quat conjugate() const;
};

inline Quat Quat::one() {
    return Quat { { 1.0f, 0.0f, 0.0f, 0.0f } };
}

Quat operator*(Quat x, Quat y);
Quat &operator*=(Quat &x, Quat y);

}
#endif
