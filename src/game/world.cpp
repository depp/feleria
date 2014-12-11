/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "world.hpp"
#include "base/chunk.hpp"
#include <cstring>
namespace Game {

namespace {

const int MAX_TILE = 6;
const int SCAN_SIZE = 3;
const World::EdgeTrace TRACE_OUTSIDE((float) -(SCAN_SIZE + 1), Vec2::zero());
const World::EdgeTrace TRACE_INSIDE ((float) +(SCAN_SIZE + 1), Vec2::zero());

const unsigned char TILE_CONVERT[2][MAX_TILE + 1] = {
    { 0, 1, 2, 3, 4, 5, 0 }, // not player
    { 0, 1, 2, 3, 4, 5, 1 }, // player
};

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
      m_heightmap(nullptr),
      m_tilemap(nullptr) {
    m_vertex_scale = Vec3{{1.0f, 1.0f, 1.0f}};
}

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
        w.m_height_scale =
            (sz->height_max - sz->height_min) * (1.0f / 255.0f);
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
        for (const unsigned char *p = w.m_tilemap, *e = p + chunk.second;
             p != e; p++) {
            if (*p > MAX_TILE) {
                return false;
            }
        }
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

World::EdgeTrace World::edge_distance(Vec2 pos, bool is_player) const {
    Vec2 rpos = pos + m_center;
    int x = (int) rpos[0], y = (int) rpos[1];
    int w = m_size[0], h = m_size[1];
    if (x < 0 || x >= w || y < 0 || y >= h) {
        return TRACE_OUTSIDE;
    }
    float fx = std::fmod(rpos[0], 1.0f), fy = std::fmod(rpos[1], 1.0f);
    const unsigned char *tc = TILE_CONVERT[(int) is_player];
    int tile = tc[m_tilemap[y*w+x]];
    bool is_inside;
    switch (tile) {
    default:
    case 0: is_inside = false; break;
    case 1: is_inside = true; break;
    case 2: is_inside = fy > 1.0f - fx; break;
    case 3: is_inside = fy > fx; break;
    case 4: is_inside = fy < fx; break;
    case 5: is_inside = fy < 1.0f - fx; break;
    }

    int x0 = x - SCAN_SIZE, x1 = x + 1 + SCAN_SIZE;
    int y0 = y - SCAN_SIZE, y1 = y + 1 + SCAN_SIZE;
    if (x0 < 0) x0 = 0;
    if (x1 >= w) x1 = w;
    if (y0 < 0) y0 = 0;
    if (y1 >= h) y1 = h;
    int ignore = is_inside ? 1 : 0;
    EdgeTrace best = TRACE_INSIDE;
    float best2 = best.first * best.first;
    for (int yi = y0; yi < y1; yi++) {
        for (int xi = x0; xi < x1; xi++) {
            tile = tc[m_tilemap[yi*w+xi]];
            EdgeTrace ttrace;
            Vec2 vp;
            if (tile < 2) {
                if (tile == ignore) {
                    continue;
                }
                if (x == xi) {
                    if (y > yi) {
                        ttrace = EdgeTrace(
                            (float) (y - yi - 1) + fy,
                            Vec2 {{ 0.0f, -1.0f }});
                    } else {
                        ttrace = EdgeTrace(
                            (float) (yi - y) - fy,
                            Vec2 {{ 0.0f, +1.0f }});
                    }
                    goto have_trace;
                }
                if (y == yi) {
                    if (x > xi) {
                        ttrace = EdgeTrace(
                            (float) (x - xi - 1) + fx,
                            Vec2 {{ -1.0f, 0.0f }});
                    } else {
                        ttrace = EdgeTrace(
                            (float) (xi - x) - fx,
                            Vec2 {{ +1.0f, 0.0f }});
                    }
                    goto have_trace;
                }
                vp[0] = (float) (xi + (x > xi));
                vp[1] = (float) (yi + (y > yi));
                goto point_distance;
            } else {
                float a, b;
                Vec2 dp;
                switch (tile) {
                case 2:
                case 5:
                    vp[0] = (float) (xi + 1);
                    vp[1] = (float) yi;
                    dp = vp - rpos;
                    b = dp[1] - dp[0];
                    a = dp[1] + dp[0];
                    if (b > 0.0f) {
                        goto point_distance;
                    }
                    if (b < -2.0f) {
                        vp[0] += -1.0f;
                        vp[1] += +1.0f;
                        goto point_distance;
                    }
                    ttrace = EdgeTrace(
                        std::abs(a) * std::sqrt(0.5f),
                        Vec2 {{ 1.0f, 1.0f }} *
                        std::copysign(std::sqrt(0.5f), a));
                    goto have_trace;

                case 3:
                case 4:
                    vp[0] = (float) xi;
                    vp[1] = (float) yi;
                    dp = vp - rpos;
                    b = dp[1] + dp[0];
                    a = dp[1] - dp[0];
                    if (b > 0.0f) {
                        goto point_distance;
                    }
                    if (b < -2.0f) {
                        vp[0] += +1.0f;
                        vp[1] += +1.0f;
                        goto point_distance;
                    }
                    ttrace = EdgeTrace(
                        std::abs(a) * std::sqrt(0.5f),
                        Vec2 {{ -1.0f, 1.0f }} *
                        std::copysign(std::sqrt(0.5f), a));
                    goto have_trace;
                }
                (void) a;
                continue;
            }

        point_distance:
            {
                Vec2 dp = vp - rpos;
                float d2 = dp.mag2();
                if (d2 < best2) {
                    float d = std::sqrt(d2);
                    if (d < 1e-4) {
                        best = EdgeTrace(d, Vec2::zero());
                    } else {
                        best = EdgeTrace(d, dp * (1.0f / d));
                    }
                    best2 = d2;
                }
            }
            continue;

        have_trace:
            if (ttrace.first < best.first) {
                best = ttrace;
                best2 = ttrace.first * ttrace.first;
            }
        }
    }
    return is_inside ? best : EdgeTrace(-best.first, -best.second);
}

}
