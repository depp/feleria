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
using Game::SPRITE_COUNT;

namespace {

const char SPRITE_MAGIC[16] = "Feleria Sprites";

struct FileGroupInfo {
    unsigned short w, h, offset;
};

const sg_sprite ZERO_SPRITE = { 0, 0, 0, 0, 0, 0 };

}

struct SpriteSheet::GroupInfo {
    int w, h;
    const sg_sprite *sprites;
};

SpriteSheet::SpriteSheet() {
    GroupInfo gifo { 0, 0, nullptr };
    m_group.resize(SPRITE_COUNT, gifo);
}

SpriteSheet::~SpriteSheet() { }

bool SpriteSheet::load() {
    Base::Texture texture;
    if (!texture.load("image/sprite")) {
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, texture.tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    Base::Data data;
    data.read("image/sprite.sgsprite", 1u << 20);
    Base::ChunkReader chunks;
    if (!chunks.read(data) ||
        std::memcmp(chunks.magic(), SPRITE_MAGIC, sizeof(SPRITE_MAGIC)) ||
        chunks.version().first != 1) {
        return false;
    }

    auto chunk_gnam = chunks.get("GNAM");
    auto chunk_gifo = chunks.get("GIFO");
    auto chunk_sprt = chunks.get("SPRT");
    std::size_t gcount = chunk_gnam.second / 16;
    if (chunk_gifo.second != gcount * 6) {
        return false;
    }
    std::size_t scount = chunk_sprt.second / 12;
    const char (*data_gnam)[16] =
        reinterpret_cast<const char (*)[16]>(chunk_gnam.first);
    const FileGroupInfo *data_gifo =
        reinterpret_cast<const FileGroupInfo *>(chunk_gifo.first);
    const sg_sprite *data_sprt =
        reinterpret_cast<const sg_sprite *>(chunk_sprt.first);

    std::vector<GroupInfo> ginfo;
    ginfo.reserve(SPRITE_COUNT);
    for (int i = 0; i < SPRITE_COUNT; i++) {
        GroupInfo gr { 0, 0, nullptr };
        std::size_t j;
        for (j = 0; j < gcount; j++) {
            if (std::strcmp(Game::SPRITE_NAMES[i], data_gnam[j]))
                continue;
            const auto &g = data_gifo[j];
            if (g.w == 0 || g.h == 0) {
                break;
            }
            std::size_t count = g.w * g.h;
            if (g.offset > scount || count > scount - g.offset) {
                return false;
            }
            gr.w = g.w;
            gr.h = g.h;
            gr.sprites = data_sprt + g.offset;
            break;
        }
        ginfo.push_back(gr);
    }

    m_data = std::move(data);
    m_group = std::move(ginfo);
    m_texture = std::move(texture);
    return true;
}

const struct sg_sprite &SpriteSheet::get(
    Game::Sprite sprite, int nx, int ny) const {
    int idx = static_cast<int>(sprite);
    if (idx < 0 || idx >= SPRITE_COUNT)
        Log::abort("Invalid sprite.");
    const auto &ifo = m_group[idx];
    if (nx < 0 || nx >= ifo.w || ny < 0 || ny >= ifo.h)
        return ZERO_SPRITE;
    return ifo.sprites[ny * ifo.w + nx];
}

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
