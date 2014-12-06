/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_RANDOM_HPP
#define LD_BASE_RANDOM_HPP
namespace Base {

struct Random {
    static Random global;

    unsigned x, y, z, w;

    static unsigned gnext() {
        return global.next();
    }

    unsigned next() {
        unsigned xx = x;
        unsigned tt = xx ^ (xx << 11);
        x = y;
        y = z;
        z = w;
        unsigned ww = w;
        w = ww ^ (ww >> 19) ^ (tt ^ (tt >> 8));
        return ww;
    }

    // generates x in 0 <= x < max
    int nexti(int max);

    // generates x in 0 <= x < max
    static int gnexti(int max) {
        return global.nexti(max);
    }

    static float gnextf() {
        return global.nextf();
    }

    float nextf() {
        return (float) next() * (1.0f / 4294967296.0f);
    }

    static double gnextd() {
        return global.nextf();
    }

    double nextd() {
        unsigned x, y;
        x = next();
        y = next();
        double v = x * (1.0 / 4294967296.0);
        return (v + y) * (1.0 / 4294967296.0);
    }

    void init();
};

}
#endif
