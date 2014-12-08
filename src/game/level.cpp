/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "level.hpp"
#include "base/chunk.hpp"
#include "sprite.hpp"
#include <cstring>
namespace Game {

namespace {

const char LEVEL_MAGIC[16] = "Feleria Level";

struct FSpawnType {
    short sprite[SpawnPoint::SPRITE_COUNT];
};

struct FSpawnPoint {
    short pos[2];
    unsigned short type;
};

}

bool Level::load(const SpriteData &sprites, const std::string &name) {
    std::string path("level/");
    path += name;
    path += ".dat";
    Base::Data data;
    data.read(path, 1024 * 1024);
    Base::ChunkReader chunks;
    if (!chunks.read(data) ||
        std::memcmp(chunks.magic(), LEVEL_MAGIC, 16) ||
        chunks.version().first != 1) {
        return false;
    }

    std::vector<int> spr_idx;
    {
        auto dsprt = chunks.get_array<char[16]>("SPRT");
        spr_idx.reserve(dsprt.size());
        for (const auto &name : dsprt) {
            if (!std::memchr(name, '\0', sizeof(name))) {
                return false;
            }
            int idx = sprites.get_index(name);
            spr_idx.push_back(idx);
        }
    }

    std::vector<SpawnPoint> stypes;
    {
        auto dtype = chunks.get_array<FSpawnType>("TYPE");
        stypes.reserve(dtype.size());
        for (const auto &st : dtype) {
            SpawnPoint pt;
            pt.pos = Vec2::zero();
            for (int i = 0; i < SpawnPoint::SPRITE_COUNT; i++) {
                int idx = st.sprite[i];
                if (idx != -1) {
                    if (idx < 0 || (std::size_t) idx >= spr_idx.size())
                        return false;
                    idx = spr_idx[idx];
                }
                pt.sprite[i] = idx;
            }
            stypes.push_back(pt);
        }
    }

    std::vector<SpawnPoint> spawn;
    {
        auto chunk = chunks.get_array<FSpawnPoint>("SPWN");
        spawn.reserve(chunk.size());
        for (const auto &lsp : chunk) {
            int idx = lsp.type;
            if ((std::size_t) idx >= stypes.size()) {
                return false;
            }
            SpawnPoint pt = stypes[idx];
            pt.pos = Vec2 {{ (float) lsp.pos[0] + 0.5f,
                             (float) lsp.pos[1] + 0.5f }};
            spawn.push_back(pt);
        }
    }

    m_spawn = std::move(spawn);
    return true;
}

}
