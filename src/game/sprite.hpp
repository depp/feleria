/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_SPRITE_HPP
#define LD_GAME_SPRITE_HPP
#include "base/file.hpp"
#include "base/range.hpp"
#include <vector>
struct sg_sprite;
namespace Game {

/// Information about all sprites in the assets.
class SpriteData {
private:
    struct GroupInfo {
        int w, h;
        const sg_sprite *sprites;
    };

    Base::Data m_data;
    std::vector<GroupInfo> m_groupinfo;
    Base::Range<char[16]> m_groupname;

public:
    // ============================================================
    // Entry points
    // ============================================================

    /// Load the sprite data.
    bool load();

    // ============================================================
    // Queries
    // ============================================================

    /// Get sprite index by name.
    int get_index(const char *name) const;

    /// Get sprite data.
    const struct sg_sprite &get_data(int sprite, int nx, int ny) const;
};

}
#endif
