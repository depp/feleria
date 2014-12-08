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

struct SizeInfo {
    unsigned short w, h;
    float height_min, height_max;
    Vec3 vert_scale;
};

}

World::World()
    : m_data(),
      m_vertex_data(std::pair<const void *, std::size_t>(nullptr, 0)),
      m_size(IVec2::zero()),
      m_center(Vec2::zero()),
      m_height_min(-1.0f),
      m_height_max(+1.0f),
      m_height_scale(1.0f),
      m_vertex_scale{{ 1.0f, 1.0f, 1.0f }},
      m_heightmap(nullptr),
      m_tilemap(nullptr) { }

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
        if (chunk.second != sizeof(struct SizeInfo)) {
            return false;
        }
        const SizeInfo *sz =
            reinterpret_cast<const SizeInfo *>(chunk.first);
        w.m_size = IVec2{{ sz->w, sz->h }};
        w.m_center = Vec2{{ (float) sz->w * 0.5f, (float) sz->h * 0.5f }};
        w.m_height_min = sz->height_min;
        w.m_height_max = sz->height_max;
        w.m_height_scale = (sz->height_max - sz->height_min) * (1.0f / 255.0f);
        w.m_vertex_scale = sz->vert_scale;
    }

    {
        auto chunk = chunks.get("MESH");
        if (!chunk.second) {
            return false;
        }
        w.m_vertex_data = chunk;
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

float World::height_at(Vec2 pos) const {
    Vec2 rpos = pos + m_center;
    int x = (int) rpos[0], y = (int) rpos[1];
    int w = m_size[0], h = m_size[1];
    if (x < 0 || y < 0 || x >= w - 1 || y >= h - 1)
        return m_height_min;
    float v00 = m_heightmap[(y+0)*w+x+0];
    float v01 = m_heightmap[(y+0)*w+x+1];
    float v10 = m_heightmap[(y+1)*w+x+0];
    float v11 = m_heightmap[(y+1)*w+x+1];
    float fx = std::fmod(rpos[0], 1.0f), fy = std::fmod(rpos[1], 1.0f);
    float v0 = v00 + (v01 - v00) * fx;
    float v1 = v10 + (v11 - v10) * fx;
    return (v0 + (v1 - v0) * fy) * m_height_scale + m_height_min;
}

}
