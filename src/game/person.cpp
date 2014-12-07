/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "person.hpp"

namespace Game {

namespace {

const float MOVE_SPEED      = 12.0f;    // unit/s
const float ACCELERATION    = 50.0f;    // unit/s^2
const float STEP_DISTANCE   = 1.0f;     // unit
const float STAND_TIME      = 1.0;      // s

// Map from parts to animation groups.
const Group PART_GROUP[PART_COUNT] = {
    Group::TORSO, Group::TORSO, Group::LEGS, Group::TORSO,
    Group::NONE, Group::FACE, Group::NONE
};

// Information about each frame of the walk cycle.
const int WALK_COUNT = 4;
const int WALK_STAND = 4;
struct WalkFrame {
    unsigned char torso_frame;
    unsigned char legs_frame;
    unsigned char yoff; // applies to everything but bottom part
};
const WalkFrame WALK_FRAME[WALK_COUNT + 1] = {
    // Walking
    { 3, 1, 0 },
    { 2, 2, 1 },
    { 3, 3, 0 },
    { 4, 4, 1 },
    // Standing
    { 3, 0, 0 }
};

#define PARTS_BODY Part::BOTTOM, Part::TOP, Part::HEAD, Part::FACE, Part::HAT
const Part PART_ORDER[4][PART_COUNT] = {
    { Part::ITEM1, PARTS_BODY, Part::ITEM2 }, // Right
    { Part::ITEM2, Part::ITEM1, PARTS_BODY }, // Up
    { Part::ITEM1, PARTS_BODY, Part::ITEM2 }, // Left
    { PARTS_BODY, Part::ITEM1, Part::ITEM2 }  // Down
};
#undef PARTS_BODY

}

Person::Person(Vec2 pos, Direction dir) {
    m_in_flags = 0;
    m_in_move = Vec2::zero();
    m_dir = dir;
    for (int i = 0; i < PART_COUNT; i++)
        m_part[i] = -1;
    m_spritecount = 0;
    m_pos[0] = m_pos[1] = pos;
    m_posz[0] = m_posz[1] = 0.0f;
    m_vel = Vec2::zero();
    m_steppos = pos;
    m_stepframe = 0;
    m_standtime = -2.0 * STAND_TIME;
}

void Person::update(double abstime, float dtime) {
    // Calculate velocity.
    Vec2 v0, v1;
    {
        v0 = m_vel;
        Vec2 vmove = m_in_move * MOVE_SPEED;
        Vec2 dv = vmove - v0;
        float dvmag = dv.mag();
        float faccel = dtime * ACCELERATION;
        v1 = dvmag <= faccel ? vmove : v0 + dv * (faccel / dvmag);
    }

    // Update position and input fields.
    m_in_flags = 0;
    m_in_move = Vec2::zero();
    m_pos[0] = m_pos[1];
    m_pos[1] = m_pos[1] + (v0 + v1) * (0.5 * dtime);
    m_vel = v1;

    // Update sprites
    {
        // The offsets are always initialized by the walking code.
        short offsets[PART_COUNT];
        short frames[GROUP_COUNT];
        for (int i = 0; i < GROUP_COUNT; i++) {
            frames[i] = 0;
        }

        // Handle walking animation.
        {
            Vec2 step = m_pos[1] - m_steppos;
            float stepd2 = step.mag2();
            WalkFrame walk;
            if (stepd2 >= STEP_DISTANCE * STEP_DISTANCE) {
                float stepd = std::sqrt(stepd2);
                float nstep = stepd / STEP_DISTANCE;
                m_steppos += step *
                    (std::floor(nstep) * STEP_DISTANCE / stepd);
                m_stepframe = (m_stepframe + (int) nstep) % WALK_COUNT;
                walk = WALK_FRAME[m_stepframe];
                m_standtime = abstime + STAND_TIME;
                m_dir = direction_from_vec(step);
            } else if (abstime > m_standtime) {
                walk = WALK_FRAME[WALK_STAND];
            } else {
                walk = WALK_FRAME[m_stepframe];
            }

            frames[static_cast<int>(Group::LEGS)] = walk.legs_frame;
            frames[static_cast<int>(Group::TORSO)] = walk.torso_frame;
            for (int i = 0; i < PART_COUNT; i++) {
                offsets[i] = walk.yoff;
            }
            offsets[static_cast<int>(Part::BOTTOM)] = 0;
        }

        // Render parts to the sprite array.
        {
            int pos = 0, d = static_cast<int>(m_dir);
            for (int i = 0; i < PART_COUNT; i++) {
                int part = static_cast<int>(PART_ORDER[d][i]);
                int sprite = m_part[part];
                if (sprite < 0)
                    continue;
                Group group = PART_GROUP[part];
                m_sprite[pos++] = PartSprite::create(
                    static_cast<Sprite>(sprite),
                    frames[static_cast<int>(group)],
                    0,
                    offsets[part]);
            }
            m_spritecount = pos;
        }
    }
}

}
