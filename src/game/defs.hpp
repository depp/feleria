/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_DEFS_HPP
#define LD_GAME_DEFS_HPP
#include "base/vec.hpp"
#include "base/quat.hpp"
#include "base/mat.hpp"
#include "base/ivec.hpp"
#include "base/ibox.hpp"
#include "base/log.hpp"
#include <cmath>
namespace Game {
using ::Base::Log;
using ::Base::Vec3;
using ::Base::Vec2;
using ::Base::Quat;
using ::Base::Mat4;
using ::Base::IVec2;

/// Direction for sprites.
enum class Direction { RIGHT, UP, LEFT, DOWN };

/// Convert a vector to a direction.
Direction direction_from_vec(Vec2 dir);

/// Convert a direction to a vector.
Vec2 vec_from_direction(Direction dir);

}
#endif
