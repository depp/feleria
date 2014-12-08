/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "defs.hpp"
#include "sprite.hpp"
#include "sg/sprite.h"
#include "base/chunk.hpp"
#include <cstring>
namespace Graphics {

SpriteArray::SpriteArray()
{ }

SpriteArray::SpriteArray(SpriteArray &&other)
    : m_array(std::move(other.m_array))
{ }

SpriteArray::~SpriteArray()
{ }

SpriteArray &SpriteArray::operator=(SpriteArray &&other) {
    m_array = std::move(other.m_array);
    return *this;
}

void SpriteArray::clear() {
    m_array.clear();
}

void SpriteArray::add(const SpritePart *parts, int count,
                      Vec3 pos, Vec3 right, Vec3 up,
                      Base::Orientation orient) {
    Vec3 nright, nup;

    {
        int oidx = static_cast<int>(orient);
        Vec3 up2 = (oidx & 4) != 0 ? -up : up;
        switch (oidx & 3) {
        default:
        case 0: nright =  right; nup =    up2; break;
        case 1: nright =    up2; nup = -right; break;
        case 2: nright = -right; nup =   -up2; break;
        case 3: nright =   -up2; nup =  right; break;
        }
    }

    Vertex *all_verts = m_array.insert(6 * count);
    for (int i = 0; i < count; i++) {
        Vertex *v = all_verts + 6 * i;
        const auto sp = *parts[i].sprite;

        {
            short tx0 = sp.x, tx1 = sp.x + sp.w;
            short ty1 = sp.y, ty0 = sp.y + sp.h;

            v[0].tx = tx0; v[0].ty = ty0;
            v[1].tx = tx1; v[1].ty = ty0;
            v[2].tx = tx0; v[2].ty = ty1;
            v[3].tx = tx0; v[3].ty = ty1;
            v[4].tx = tx1; v[4].ty = ty0;
            v[5].tx = tx1; v[5].ty = ty1;
        }

        {
            Vec3 spos = pos +
                parts[i].offset[0] * right + parts[i].offset[1] * up;
            Vec3 vx0 = nright * (float) -sp.cx;
            Vec3 vx1 = nright * (float) (sp.w - sp.cx);
            Vec3 vy0 = nup * (float) (sp.cy - sp.h);
            Vec3 vy1 = nup * (float) +sp.cy;

            v[0].pos = spos + vx0 + vy0;
            v[1].pos = v[4].pos = spos + vx1 + vy0;
            v[2].pos = v[3].pos = spos + vx0 + vy1;
            v[5].pos = spos + vx1 + vy1;
        }
    }
}

void SpriteArray::upload(GLuint usage) {
    m_array.upload(usage);
}

}
