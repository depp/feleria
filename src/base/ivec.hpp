/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_IVEC_HPP
#define LD_BASE_IVEC_HPP
namespace Base {

/// Integer vector.
template<int N>
struct IVec {
    int v[N];

    int operator[](int i) const { return v[i]; }
    int &operator[](int i) { return v[i]; }

    /// The zero vector.
    static IVec zero();
};

template<int N>
IVec<N> operator+(IVec<N> u) {
    return u;
}

template<int N>
IVec<N> operator+(IVec<N> u, IVec<N> v) {
    IVec<N> r;
    for (int i = 0; i < N; i++)
        r[i] = u[i] + v[i];
    return r;
}

template<int N>
IVec<N> &operator+=(IVec<N> &u, IVec<N> v) {
    for (int i = 0; i < N; i++)
        u[i] += v[i];
    return u;
}

template<int N>
IVec<N> operator-(IVec<N> v) {
    IVec<N> r;
    for (int i = 0; i < N; i++)
        r[i] = -v[i];
    return r;
}

template<int N>
IVec<N> operator-(IVec<N> u, IVec<N> v) {
    IVec<N> r;
    for (int i = 0; i < N; i++)
        r[i] = u[i] - v[i];
    return r;
}

template<int N>
IVec<N> &operator-=(IVec<N> &u, IVec<N> v) {
    for (int i = 0; i < N; i++)
        u[i] -= v[i];
    return u;
}

template<int N>
IVec<N> operator*(int a, IVec<N> v) {
    IVec<N> r;
    for (int i = 0; i < N; i++)
        r[i] = a * v[i];
    return r;
}

template<int N>
IVec<N> operator*(IVec<N> v, int a) {
    IVec<N> r;
    for (int i = 0; i < N; i++)
        r[i] = v[i] * a;
    return r;
}

template<int N>
IVec<N> &operator*=(IVec<N> &u, int a) {
    for (int i = 0; i < N; i++)
        u[i] *= a;
    return u;
}

template<int N>
IVec<N> IVec<N>::zero() {
    IVec r;
    for (int i = 0; i < N; i++)
        r[i] = 0.0f;
    return r;
}

typedef IVec<2> IVec2;
typedef IVec<3> IVec3;

}
#endif
