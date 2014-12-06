/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_IBOX_HPP
#define LD_BASE_IBOX_HPP
#include "ivec.hpp"
#include <algorithm>
namespace Base {

/// N-dimensional axis-aligned box with integer coordinates.
template<int N>
struct IBox {
    IVec<N> mins, maxs;

    /// A box with zero volume.
    static IBox zero();
    /// Create the smallest box containing both boxes.
    static IBox union_(const IBox &a, const IBox &b);
    /// Create the largest box contained by both boxes.
    static IBox intersection(const IBox &a, const IBox &b);
    /// Whether or not the box has zero volume.
    bool empty() const;
    /// Whether or not this box contains a vector.
    bool contains(const IVec<N> &v) const;
    /// Whether or not this box contains another.
    bool contains(const IBox &box) const;
    /// Get the volume of the box.
    int volume() const;
};

template<int N>
IBox<N> operator+(const IBox<N> &b, const IVec3 &v) {
    return IBox<N> { b.mins + v, b.maxs + v };
}

template<int N>
IBox<N> &operator+=(const IBox<N> &b, const IVec3 &v) {
    b.mins += v;
    b.maxs += v;
    return b;
}

template<int N>
IBox<N> operator-(const IBox<N> &b, const IVec3 &v) {
    return IBox<N> { b.mins - v, b.maxs - v };
}

template<int N>
IBox<N> &operator-=(const IBox<N> &b, const IVec3 &v) {
    b.mins -= v;
    b.maxs -= v;
    return b;
}

template<int N>
IBox<N> IBox<N>::zero() {
    return IBox<N> { IVec<N>::zero(), IVec<N>::zero() };
}

template<int N>
IBox<N> IBox<N>::union_(const IBox<N> &a, const IBox<N> &b) {
    IBox<N> r;
    for (int i = 0; i < N; i++) {
        r.mins[i] = std::min(a.mins[i], b.mins[i]);
        r.maxs[i] = std::max(a.maxs[i], b.maxs[i]);
    }
    return r;
}

template<int N>
IBox<N> IBox<N>::intersection(const IBox<N> &a, const IBox<N> &b) {
    IBox<N> r;
    for (int i = 0; i < N; i++) {
        r.mins[i] = std::max(a.mins[i], b.mins[i]);
        r.maxs[i] = std::min(a.maxs[i], b.maxs[i]);
    }
    return r;
}

template<int N>
bool IBox<N>::empty() const {
    for (int i = 0; i < N; i++)
        if (mins[i] >= maxs[i])
            return true;
    return false;
}

template<int N>
bool IBox<N>::contains(const IVec<N> &v) const {
    for (int i = 0; i < N; i++)
        if (mins[i] > v[i] || maxs[i] <= v[i])
            return false;
    return true;
}

template<int N>
bool IBox<N>::contains(const IBox<N> &box) const {
    for (int i = 0; i < N; i++)
        if (mins[i] > box.mins[i] || maxs[i] < box.maxs[i])
            return false;
    return true;
}

template<int N>
int IBox<N>::volume() const {
    int v = 1;
    for (int i = 0; i < N; i++)
        v *= maxs[i] - mins[i];
    return v;
}

typedef IBox<2> IBox2;
typedef IBox<3> IBox3;

}
#endif
