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
      m_size(IVec2::zero()) { }

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
    }

    {
        auto chunk = chunks.get("MESH");
        if (!chunk.second) {
            return false;
        }
        w.m_vertex_data = chunk.first;
        w.m_vertex_size = chunk.second;
    }

    *this = std::move(w);
    return true;
}

}
