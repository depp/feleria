/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "world.hpp"
#include "base/chunk.hpp"
#include <cstring>
namespace Game {

namespace {

const char WORLD_MAGIC[16] = "Feleria World";

}

World::World()
    : m_vertex_data(nullptr), m_vertex_size(0),
      m_size(IVec2::zero()), m_center(Vec2::zero()),
      m_heightmap(nullptr) { }

bool World::load() {
    World w;
    w.m_data.read("world.dat", 1u << 24);
    Base::ChunkReader chunks;
    if (!chunks.read(w.m_data) ||
        std::memcmp(chunks.magic(), WORLD_MAGIC, 16) ||
        chunks.version().first != 1) {
        return false;
    }

    {
        auto chunk = chunks.get("SIZE");
        if (chunk.second != 4) {
            return false;
        }
        const unsigned short *d =
            reinterpret_cast<const unsigned short *>(chunk.first);
        w.m_size = IVec2{{ d[0], d[1] }};
        w.m_center = Vec2{{ (float) d[0] * 0.5f, (float) d[1] * 0.5f }};
    }

    {
        auto chunk = chunks.get("MESH");
        if (!chunk.second) {
            return false;
        }
        w.m_vertex_data = chunk.first;
        w.m_vertex_size = chunk.second;
    }

    std::size_t tilesz = (std::size_t) w.m_size[0] * w.m_size[1];

    {
        auto chunk = chunks.get("HGHT");
        if (chunk.second != tilesz) {
            return false;
        }
        w.m_heightmap = reinterpret_cast<const unsigned char *>(chunk.first);
    }

    {
        auto chunk = chunks.get("TILE");
        if (chunk.second != tilesz) {
            return false;
        }
        w.m_tilemap = reinterpret_cast<const unsigned char *>(chunk.first);
    }

    *this = std::move(w);
    return true;
}

float World::height(Vec2 pos) const {
    Vec2 rpos = pos - m_center;
    int x = (int) rpos[0], y = (int) rpos[1];
    int w = m_size[0], h = m_size[1];
    if (x < 0 || y < 0 || x >= w - 1 || y >= h - 1)
        return 0.0f;
    float v00 = m_heightmap[(y+0)*w+x+0];
    float v01 = m_heightmap[(y+0)*w+x+1];
    float v10 = m_heightmap[(y+1)*w+x+0];
    float v11 = m_heightmap[(y+1)*w+x+1];
    float fx = std::fmod(rpos[0], 1.0f), fy = std::fmod(rpos[1], 1.0f);
    float v0 = v00 + (v01 - v00) * fx;
    float v1 = v10 + (v11 - v10) * fx;
    return v0 + (v1 - v0) * fy;
}

}
