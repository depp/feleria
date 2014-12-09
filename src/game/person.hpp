/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_PERSON_HPP
#define LD_GAME_PERSON_HPP
#include "defs.hpp"
#include "sprite.hpp"
#include "base/range.hpp"
namespace Game {
class Game;

// Parts of a person.
static const int PART_COUNT = 8;
enum class Part {
    ITEM1,
    ITEM2,
    BOTTOM,
    TOP,
    HEAD,
    FACE,
    HAT,
    ONLY
};

// Animation groups
static const int GROUP_COUNT = 4;
enum Group {
    NONE, // no animation
    LEGS, // bottom
    TORSO, // items and top
    FACE, // face
};

// Information about a part of a person.
class PartSprite {
private:
    unsigned m_data;

public:
    // Create a part of a person with an offset.
    static PartSprite create(int sprite, int frame, int offx, int offy) {
        PartSprite s;
        s.m_data = ((sprite & 0xff) |
                    ((frame & 0xff) << 8) |
                    (((offx + 8) & 0xf) << 16) |
                    (((offy + 8) & 0xf) << 20));
        return s;

    }

    // Get the sprite group for this part.
    int sprite() const {
        return m_data & 0xff;
    }

    // Get the current frame for this part.
    int frame() const {
        return (m_data >> 8) & 0xff;
    }

    // Get the offset at which the sprite should be rendered.
    Vec2 offset() const {
        return Vec2 {{
            (float) ((int) ((m_data >> 16) & 0xf) - 8),
            (float) ((int) ((m_data >> 20) & 0xf) - 8)
        }};
    }
};

/// A person in the game, in a broad sense.  This includes monsters,
/// the player, and NPCs.
class Person {
public:
    // Flags for updating a person.
    enum {
        FL_ATTACK = 01
    };

private:
    int m_identity;
    bool m_is_player;

    // The current facing direction.
    Direction m_dir;

    // Sprites for each part of the person.
    int m_part[PART_COUNT];

    // Expanded version of the sprites, recalculated each update.
    PartSprite m_sprite[PART_COUNT];
    int m_spritecount;

    // Current and previous position.
    Vec2 m_pos[2];
    float m_posz[2];
    // Current velocity.
    Vec2 m_vel;

    // The location where the walking animation last advanced.
    Vec2 m_steppos;
    // The current frame of the walking animation.
    int m_stepframe;
    // Counter until walking turns into standing.
    float m_standtime;

public:
    // ============================================================
    // Entry points
    // ============================================================

    /// Create a person at the given location.
    Person(int identity, Vec2 pos, Direction dir);

    /// Initialize the person's state.  Called exactly once, after the
    /// person is added to the game.
    void initialize(Game &game);

    /// Update the person's state.  Should be called exactly once per
    /// game update tick.
    void update(Game &game);

    // ============================================================
    // Modifying
    // ============================================================

    /// Set the apperance of a part of the person.
    void set_part(Part part, int sprite) {
        m_part[static_cast<int>(part)] = sprite;
    }

    void set_player(bool is_player) {
        m_is_player = is_player;
    }

    // ============================================================
    // Queries
    // ============================================================

    int identity() const {
        return m_identity;
    }

    /// Get the current facing direction.
    Direction direction() const {
        return m_dir;
    }

    // Get the current position.
    Vec3 position(float frac) const {
        return Vec3{{
            m_pos[0][0] + frac * (m_pos[1][0] - m_pos[0][0]),
            m_pos[0][1] + frac * (m_pos[1][1] - m_pos[0][1]),
            m_posz[0] + frac * (m_posz[1] - m_posz[0])
        }};
    }

    // Get the person's sprites.
    Base::Range<PartSprite> sprite() const {
        return Base::Range<PartSprite>(
            m_sprite, m_sprite + m_spritecount);
    }
};

}
#endif
