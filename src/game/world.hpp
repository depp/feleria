/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_WORLD_HPP
#define LD_GAME_WORLD_HPP
#include "base/file.hpp"
#include "defs.hpp"
#include <utility>
namespace Game {

/// Information about the world (the terrain, not the objects in it).
class World {
public:
    typedef std::pair<float, Vec2> EdgeTrace;

private:
    Base::Data m_data;
    std::pair<const void *, std::size_t> m_vertex_data;
    IVec2 m_size;
    Vec2 m_center;
    float m_height_min, m_height_max, m_height_scale;
    Vec3 m_vertex_scale;
    const unsigned char *m_heightmap;
    const unsigned char *m_tilemap;

public:
    // ============================================================
    // Entry points
    // ============================================================

    World();

    /// Load the world data.
    bool load();

    // ============================================================
    // Queries
    // ============================================================

    /// Get the vertex data.
    std::pair<const void *, std::size_t> vertex_data() const {
        return m_vertex_data;
    }

    /// Get the scaling factor to apply to vertex positions.
    Vec3 vertex_scale() const {
        return m_vertex_scale;
    }

    /// Get the dimensions of the world.
    IVec2 size() const {
        return m_size;
    }

    /// Get the map center, in map coordinates (not world coordinates).
    Vec2 center() const {
        return m_center;
    }

    /// Get the terrain height at the given position.
    float height_at(Vec2 pos) const;

    /// Project a 2D point onto the terrain.
    Vec3 project(Vec2 pos) const {
        return Vec3 {{ pos[0], pos[1], height_at(pos) }};
    }

    /// Get the (distance, direction) to the nearest map edge.  The
    /// distance is positive if we are inside the map, and negative if
    /// we are outside.  If the magnitude of the distance is large,
    /// then the distance and direction are bogus.
    EdgeTrace edge_distance(Vec2 pos, bool is_player) const;
};

}
#endif
