/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "random.hpp"
#include <limits>
namespace Base {

Random Random::global = {
    0x20cc842bu,
    0xe2db5f92u,
    0x62bb2830u,
    0xda65a90bu
};

void Random::init() {
    x = 0x20cc842bu;
    y = 0xe2db5f92u;
    z = 0x62bb2830u;
    w = 0xda65a90bu;
}

int Random::nexti(int max) {
    if (max <= 1)
        return 0;
    unsigned bin_size = std::numeric_limits<unsigned>::max() / max;
    unsigned limit = bin_size * max - 1;
    unsigned x;
    do x = next();
    while (x > limit);
    return x / bin_size;
}

}
