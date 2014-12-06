/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_VEC_HPP
#define LD_BASE_VEC_HPP
#include <cmath>
namespace Base {

/// Floating-point vector.
template<int N>
struct Vec {
    float v[N];

    float operator[](int i) const { return v[i]; }
    float &operator[](int i) { return v[i]; }

    /// The zero vector.
    static Vec zero();
    /// Get the magnitude of the vector, squared.
    float mag2() const;
    /// Get the magnitude of the vector.
    float mag() const;
    /// Get the dot product of two vectors.
    static float dot(Vec u, Vec v);
    /// Get the distance between two vectors, squared.
    static float dist2(Vec u, Vec v);
    /// Get the distance between two vectors.
    static float dist(Vec u, Vec v);
    /// Interpolate between two vectors.
    static Vec lerp(Vec u, Vec v, float a);
};

template<int N>
bool operator==(Vec<N> u, Vec<N> v) {
    for (int i = 0; i < N; i++)
        if (u[i] != v[i])
            return false;
    return true;
}

template<int N>
bool operator!=(Vec<N> u, Vec<N> v) {
    return !(u == v);
}

template<int N>
Vec<N> operator+(Vec<N> u) {
    return u;
}

template<int N>
Vec<N> operator+(Vec<N> u, Vec<N> v) {
    Vec<N> r;
    for (int i = 0; i < N; i++)
        r[i] = u[i] + v[i];
    return r;
}

template<int N>
Vec<N> &operator+=(Vec<N> &u, Vec<N> v) {
    for (int i = 0; i < N; i++)
        u[i] += v[i];
    return u;
}

template<int N>
Vec<N> operator-(Vec<N> v) {
    Vec<N> r;
    for (int i = 0; i < N; i++)
        r[i] = -v[i];
    return r;
}

template<int N>
Vec<N> operator-(Vec<N> u, Vec<N> v) {
    Vec<N> r;
    for (int i = 0; i < N; i++)
        r[i] = u[i] - v[i];
    return r;
}

template<int N>
Vec<N> &operator-=(Vec<N> &u, Vec<N> v) {
    for (int i = 0; i < N; i++)
        u[i] -= v[i];
    return u;
}

template<int N>
Vec<N> operator*(float a, Vec<N> v) {
    Vec<N> r;
    for (int i = 0; i < N; i++)
        r[i] = a * v[i];
    return r;
}

template<int N>
Vec<N> operator*(Vec<N> v, float a) {
    Vec<N> r;
    for (int i = 0; i < N; i++)
        r[i] = v[i] * a;
    return r;
}

template<int N>
Vec<N> &operator*=(Vec<N> &u, float a) {
    for (int i = 0; i < N; i++)
        u[i] *= a;
    return u;
}

template<int N>
Vec<N> Vec<N>::zero() {
    Vec r;
    for (int i = 0; i < N; i++)
        r[i] = 0.0f;
    return r;
}

template<int N>
float Vec<N>::mag2() const {
    float a = 0.0f;
    for (int i = 0; i < N; i++)
        a += v[i] * v[i];
    return a;
}

template<int N>
float Vec<N>::mag() const {
    return std::sqrt(mag2());
}

template<int N>
float Vec<N>::dot(Vec<N> u, Vec<N> v) {
    float a = 0.0f;
    for (int i = 0; i < N; i++)
        a += u[i] * v[i];
    return a;
}

template<int N>
float Vec<N>::dist2(Vec<N> u, Vec<N> v) {
    return (u - v).mag2();
}

template<int N>
float Vec<N>::dist(Vec<N> u, Vec<N> v) {
    return (u - v).mag();
}

template<int N>
Vec<N> Vec<N>::lerp(Vec<N> u, Vec<N> v, float a) {
    return u + (v - u) * a;
}

typedef Vec<2> Vec2;
typedef Vec<3> Vec3;

}
#endif
