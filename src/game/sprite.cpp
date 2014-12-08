/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "defs.hpp"
#include "sprite.hpp"
#include "sg/sprite.h"
#include "base/chunk.hpp"
#include <cstring>
namespace Game {

namespace {

const char SPRITE_MAGIC[16] = "Feleria Sprites";

struct FGroupInfo {
    unsigned short w, h, offset;
};

const sg_sprite ZERO_SPRITE = { 0, 0, 0, 0, 0, 0 };

}

bool SpriteData::load() {
    Base::Data data;
    data.read("image/sprite.sgsprite", 1u << 20);
    Base::ChunkReader chunks;
    if (!chunks.read(data) ||
        std::memcmp(chunks.magic(), SPRITE_MAGIC, sizeof(SPRITE_MAGIC)) ||
        chunks.version().first != 1) {
        return false;
    }

    auto chunk_gnam = chunks.get_array<char[16]>("GNAM");
    auto chunk_gifo = chunks.get_array<FGroupInfo>("GIFO");
    auto chunk_sprt = chunks.get_array<sg_sprite>("SPRT");

    std::size_t scount = chunk_sprt.size();
    std::size_t gcount = chunk_gnam.size();
    if (chunk_gifo.size() != gcount) {
        return false;
    }

    std::vector<GroupInfo> ginfo(gcount);
    for (std::size_t i = 0; i < gcount; i++) {
        const auto &name = chunk_gnam[i];
        if (!std::memchr(name, 0, 16)) {
            return false;
        }
        auto &g = ginfo[i];
        const auto ifo = chunk_gifo[i];
        g.w = ifo.w;
        g.h = ifo.h;
        if (ifo.w == 0 || ifo.h == 0) {
            g.sprites = nullptr;
        } else {
            std::size_t n = (std::size_t) ifo.w * ifo.h;
            if (ifo.offset > scount || n > scount - ifo.offset) {
                return false;
            }
            g.sprites = &chunk_sprt[ifo.offset];
        }
    }

    m_data = std::move(data);
    m_groupinfo = std::move(ginfo);
    m_groupname = chunk_gnam;
    return true;
}

int SpriteData::get_index(const char *name) const {
    std::size_t i, n = m_groupname.size();
    for (i = 0; i < n; i++) {
        if (!std::strcmp(name, m_groupname[i])) {
            return (int) i;
        }
    }
    Log::warn("Missing sprite: %s", name);
    return -1;
}

const struct sg_sprite &SpriteData::get_data(
    int sprite, int nx, int ny) const {
    if (sprite < 0 || (std::size_t) sprite >= m_groupinfo.size()) {
        return ZERO_SPRITE;
    }
    const auto &ifo = m_groupinfo[sprite];
    if (nx < 0 || nx >= ifo.w || ny < 0 || ny >= ifo.h)
        return ZERO_SPRITE;
    return ifo.sprites[ny * ifo.w + nx];
}

}
