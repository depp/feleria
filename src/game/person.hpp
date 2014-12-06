/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_PERSON_HPP
#define LD_GAME_PERSON_HPP
#include "defs.hpp"
#include "sprite.hpp"
namespace Game {

// Parts of a person.
static const int PART_COUNT = 7;
enum class Part {
    ITEM1,
    ITEM2,
    BOTTOM,
    TOP,
    HEAD,
    FACE,
    HAT
};

// Animation groups
static const int GROUP_COUNT = 4;
enum Group {
    NONE, // no animation
    LEGS, // bottom
    TORSO, // items and top
    FACE, // face
};

/// A person in the game, in a broad sense.  This includes monsters,
/// the player, and NPCs.
class Person {
public:
    // Flags for updating a person.
    enum {
        FL_ATTACK = 01
    };

    // Input flags.
    unsigned m_in_flags;
    // Movement input.
    Vec2 m_in_move;

    // The current facing direction.
    Direction m_dir;
    // Sprites for each part of the person.
    int m_part[PART_COUNT];
    // Active frame for each animation group, with an extra entry at
    // the beginning set to zero.
    int m_frame[GROUP_COUNT];
    // Current and previous position.
    Vec2 m_pos[2];
    // Current velocity.
    Vec2 m_vel;

    // The location where the walking animation last advanced.
    Vec2 m_steppos;
    // The current frame of the walking animation.
    int m_stepframe;

    // Create a person at the given location.
    Person(Vec2 pos, Direction dir);

    // Update the person's state.  Should be called exactly once per
    // game update tick.
    void update(float dtime);

    // Set the apperance of a part of the person.
    void set_part(Part part, Sprite sprite) {
        m_part[static_cast<int>(part)] = static_cast<int>(sprite);
    }

    // Set the frame of an animation group.
    void set_frame(Group group, int frame) {
        m_frame[static_cast<int>(group)] = frame;
    }
};

}
#endif
