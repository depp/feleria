/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "color.hpp"
#include "sg/entry.h"
namespace Graphics {

Color Color::transparent() {
    Color c;
    for (int i = 0; i < 4; i++)
        c.v[i] = 0.0f;
    return c;
}

Color Color::palette(int index) {
    static const unsigned char PALETTE[32][3] = {
        {   0,   0,   0 }, // Black
        {  34,  32,  52 }, // Valhalla
        {  69,  40,  60 }, // Loulou
        { 102,  57,  49 }, // Oiled cedar
        { 143,  86,  59 }, // Rope
        { 223, 113,  38 }, // Tahiti gold
        { 217, 160, 102 }, // Twine
        { 238, 195, 154 }, // Pancho
        { 251, 242,  54 }, // Golden fizz
        { 153, 229,  80 }, // Atlantis
        { 106, 190,  48 }, // Christi
        {  55, 148, 110 }, // Elf green
        {  75, 105,  47 }, // Dell
        {  82,  75,  36 }, // Verdigris
        {  50,  60,  57 }, // Opal
        {  63,  63, 116 }, // Deep koamaru
        {  48,  96, 130 }, // Venice blue
        {  91, 110, 225 }, // Royal blue
        {  99, 155, 255 }, // Cornflower
        {  95, 205, 228 }, // Viking
        { 203, 219, 252 }, // Light steel blue
        { 255, 255, 255 }, // White
        { 155, 173, 183 }, // Heather
        { 132, 126, 135 }, // Topaz
        { 105, 106, 106 }, // Dim gray
        {  89,  86,  82 }, // Smokey ash
        { 118,  66, 138 }, // Clairvoyant
        { 172,  50,  50 }, // Brown
        { 217,  87,  99 }, // Mandy
        { 215, 123, 186 }, // Plum
        { 143, 151,  74 }, // Rain forest
        { 138, 111,  48 }  // Stinger
    };
    if (index < 0 || index >= 32)
        sg_sys_abort("invalid palette index");
    Color c;
    for (int i = 0; i < 3; i++)
        c.v[i] = (float)PALETTE[index][i] * (float)(1.0f / 255.0f);
    c.v[3] = 1.0f;
    return c;
}

Color Color::fade(float alpha) const {
    if (alpha > 1.0f)
        alpha = 1.0f;
    else if (alpha < 0.0f)
        alpha = 0.0f;
    Color c;
    for (int i = 0; i < 4; i++)
        c.v[i] = v[i] * alpha;
    return c;
}

Color Color::blend(const Color &a, const Color &b, float t) {
    if (t > 1.0f)
        t = 1.0f;
    else if (t < 0.0f)
        t = 0.0f;
    Color c;
    for (int i = 0; i < 4; i++)
        c.v[i] = a.v[i] * (1.0f - t) + b.v[i] * t;
    return c;
}

}
