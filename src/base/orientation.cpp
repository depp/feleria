/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "orientation.hpp"
namespace Base {

/// Compose two orientations.
Orientation operator*(Orientation x, Orientation y) {
    int xi = static_cast<int>(x), yi = static_cast<int>(y);
    int xm = (xi >> 2) & 1, ym = (yi >> 2) & 1;
    int xr = (xi & 3), yr = (yi & 3);
    if (ym)
        xr *= 3;
    return static_cast<Orientation>(((xm ^ ym) << 2) | ((xr + yr) & 3));
}

}
