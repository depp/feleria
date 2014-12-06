/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_ORIENTATION_HPP
#define LD_BASE_ORIENTATION_HPP
namespace Base {

/// Orthogonal orientations for 2D sprites.
enum class Orientation {
    // NOTE: don't change the order!
    NORMAL, ROTATE_90, ROTATE_180, ROTATE_270,
    FLIP_VERTICAL, TRANSPOSE_2, FLIP_HORIZONTAL, TRANSPOSE
};

/// Compose two orientations.
Orientation operator*(Orientation x, Orientation y);

}
#endif
