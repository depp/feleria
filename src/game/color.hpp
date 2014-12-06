/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_COLOR_HPP
#define LD_GAME_COLOR_HPP
namespace Graphics {

struct Color {
    float v[4];

    /// The transparent color.
    static Color transparent();
    /// A color from DB's 32-color palette.
    static Color palette(int index);
    /// Produce a fade between this color and black.
    /// 1.0 produces the original color, 0.0 produces transparent.
    Color fade(float alpha) const;
    /// Fade between two colors.
    static Color blend(const Color &a, const Color &b, float t);
};

}
#endif
