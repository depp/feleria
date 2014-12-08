/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_LEVEL_HPP
#define LD_GAME_LEVEL_HPP
#include "base/range.hpp"
#include "defs.hpp"
namespace Game {
class SpriteData;

/// A spawn point in a level.
struct SpawnPoint {
    static const int SPRITE_COUNT = 8;

    Vec2 pos;
    int sprite[SPRITE_COUNT];
};

/// A game level.
class Level {
public:
    typedef Base::Range<SpawnPoint> SpawnPointList;

private:
    std::vector<SpawnPoint> m_spawn;

public:
    // ============================================================
    // Entry points
    // ============================================================

    /// Load the level data.
    bool load(const SpriteData &sprites, const std::string &name);

    // ============================================================
    // Queries
    // ============================================================

    /// Iterate over spawn points.
    SpawnPointList spawn() const { return SpawnPointList(m_spawn); }
};

}
#endif
