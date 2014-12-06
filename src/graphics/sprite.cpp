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

void SpriteArray::add(const sg_sprite &sp, Vec2 pos,
                      Base::Orientation orient) {
    using Base::Orientation;
    if (sp.w == 0) {
        return;
    }

    float x = pos[0], y = pos[1];
    Vertex *data = m_array.insert(6);

    short tx0 = sp.x, tx1 = sp.x + sp.w;
    short ty1 = sp.y, ty0 = sp.y + sp.h;

    data[0].tx = tx0; data[0].ty = ty0;
    data[1].tx = tx1; data[1].ty = ty0;
    data[2].tx = tx0; data[2].ty = ty1;
    data[3].tx = tx0; data[3].ty = ty1;
    data[4].tx = tx1; data[4].ty = ty0;
    data[5].tx = tx1; data[5].ty = ty1;

    float rx0 = (float) -sp.cx, rx1 = (float) (sp.w - sp.cx);
    float ry0 = (float) (sp.cy - sp.h), ry1 = (float) +sp.cy;
    float vx[4], vy[4];
    switch (orient) {
    case Orientation::NORMAL:
        vx[0] = vx[2] = x + rx0;
        vx[1] = vx[3] = x + rx1;
        vy[0] = vy[1] = y + ry0;
        vy[2] = vy[3] = y + ry1;
        break;

    case Orientation::ROTATE_90:
        vx[2] = vx[3] = x + ry0;
        vx[0] = vx[1] = x + ry1;
        vy[2] = vy[0] = y + rx0;
        vy[3] = vy[1] = y + rx1;
        break;

    case Orientation::ROTATE_180:
        vx[3] = vx[1] = x + rx0;
        vx[2] = vx[0] = x + rx1;
        vy[3] = vy[2] = y + ry0;
        vy[1] = vy[0] = y + ry1;
        break;

    case Orientation::ROTATE_270:
        vx[1] = vx[0] = x + ry0;
        vx[3] = vx[2] = x + ry1;
        vy[1] = vy[3] = y + rx0;
        vy[0] = vy[2] = y + rx1;
        break;

    case Orientation::FLIP_VERTICAL:
        vx[0] = vx[2] = x + rx0;
        vx[1] = vx[3] = x + rx1;
        vy[0] = vy[1] = y + ry1;
        vy[2] = vy[3] = y + ry0;
        break;

    case Orientation::TRANSPOSE_2:
        vx[2] = vx[3] = x + ry0;
        vx[0] = vx[1] = x + ry1;
        vy[2] = vy[0] = y + rx1;
        vy[3] = vy[1] = y + rx0;
        break;

    case Orientation::FLIP_HORIZONTAL:
        vx[3] = vx[1] = x + rx0;
        vx[2] = vx[0] = x + rx1;
        vy[3] = vy[2] = y + ry1;
        vy[1] = vy[0] = y + ry0;
        break;

    case Orientation::TRANSPOSE:
        vx[1] = vx[0] = x + ry0;
        vx[3] = vx[2] = x + ry1;
        vy[1] = vy[3] = y + rx1;
        vy[0] = vy[2] = y + rx0;
        break;

    default:
#ifdef __GCC__
        __builtin_unreachable();
#endif
        return;
    }

    data[0].px = vx[0]; data[0].py = vy[0];
    data[1].px = vx[1]; data[1].py = vy[1];
    data[2].px = vx[2]; data[2].py = vy[2];
    data[3].px = vx[2]; data[3].py = vy[2];
    data[4].px = vx[1]; data[4].py = vy[1];
    data[5].px = vx[3]; data[5].py = vy[3];
}

void SpriteArray::upload(GLuint usage) {
    m_array.upload(usage);
}

}
