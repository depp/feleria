/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include <cstdlib>
namespace Game {

Direction direction_from_vec(Vec2 dir) {
    if (std::fabs(dir[0]) > std::fabs(dir[1])) {
        return dir[0] > 0.0 ? Direction::RIGHT : Direction::LEFT;
    } else {
        return dir[1] > 0.0 ? Direction::UP : Direction::DOWN;
    }
}

}
