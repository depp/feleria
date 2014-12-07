/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_WORLD_HPP
#define LD_GAME_WORLD_HPP
#include "base/file.hpp"
#include "defs.hpp"
namespace Game {

class World {
private:
    Base::Data m_data;
    const void *m_vertex_data;
    std::size_t m_vertex_size;
    IVec2 m_size;

public:
    World();

    /// Load the world data.
    bool load();

    /// Get the vertex data.
    const void *vertex_data() const {
        return m_vertex_data;
    }

    /// Get the size of the vertex data, in bytes.
    std::size_t vertex_size() const {
        return m_vertex_size;
    }

    /// Get the dimensions of the world.
    const IVec2 size() const {
        return m_size;
    }
};

}
#endif
