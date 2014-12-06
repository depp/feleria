/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_PERSON_HPP
#define LD_GAME_PERSON_HPP
#include "defs.hpp"
#include "sprite.hpp"
namespace Game {

/// A person in the game, in a broad sense.  This includes monsters,
/// the player, and NPCs.
class Person {
public:
    // Flags for updating a person.
    enum {
        FL_ATTACK = 01
    };

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
    static const int GROUP_COUNT = 3;
    enum Group {
        LEGS, // bottom
        TORSO, // items and top
        FACE, // face
    };

    // Input flags.
    unsigned m_in_flags;
    // Movement input.
    Vec2 m_in_move;

    // The current facing direction.
    Direction m_dir;
    // Sprites for each part of the person.
    int m_part[PART_COUNT];
    // Active frame for each animation group.
    int m_frame[GROUP_COUNT];
    // Current and previous position.
    Vec2 m_pos[2];
    // Current velocity.
    Vec2 m_vel;

    // Create a person at the given location.
    Person(Vec2 pos, Direction dir);

    // Update the person's state.  Should be called exactly once per
    // game update tick.
    void update(float dtime);

    // Set the apperance of a part of the person.
    void set_part(Part part, Sprite sprite);

    // Set the frame of an animation group.
    void set_frame(Group group, int frame);
};

}
#endif
