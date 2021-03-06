/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "person.hpp"
#include "game.hpp"

namespace Game {

namespace {

const float HEIGHT          = 3.0;      // unit
const float MOVE_SPEED      = 12.0f;    // unit/s
const float ACCELERATION    = 50.0f;    // unit/s^2
const float STEP_DISTANCE   = 1.0f;     // unit
const float STAND_TIME      = 1.0;      // s

const float PUSH_DIST = 0.25f;
const float TOUCH_DIST = 2.0f;
const float TOUCH_RADIUS = 3.01f;

// Map from parts to animation groups.
const Group PART_GROUP[PART_COUNT] = {
    Group::TORSO, Group::TORSO, Group::LEGS, Group::TORSO,
    Group::NONE, Group::FACE, Group::NONE, Group::NONE
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
    { Part::ITEM1, PARTS_BODY, Part::ITEM2, Part::ONLY }, // Right
    { Part::ITEM2, Part::ITEM1, PARTS_BODY, Part::ONLY }, // Up
    { Part::ITEM1, PARTS_BODY, Part::ITEM2, Part::ONLY }, // Left
    { PARTS_BODY, Part::ITEM1, Part::ITEM2, Part::ONLY }  // Down
};
#undef PARTS_BODY

}

Person::Person(int identity, Vec2 pos, Direction dir) {
    m_identity = identity;
    m_is_player = false;
    m_dir = dir;
    for (int i = 0; i < PART_COUNT; i++)
        m_part[i] = -1;
    m_spritecount = 0;
    m_pos[0] = m_pos[1] = pos;
    m_posz[0] = m_posz[1] = 0.0f;
    m_vel = Vec2::zero();
    m_steppos = pos;
    m_stepframe = 0;
    m_standtime = 0.0f;
}

void Person::initialize(Game &game) {
    m_posz[0] = m_posz[1] = game.world().height_at(m_pos[0]) + HEIGHT * 0.5f;
}

void Person::update(Game &game) {
    float dtime = game.frame_delta();
    Vec2 in_move;
    if (m_is_player) {
        const auto &in = game.frame_input();
        in_move = in.move;

        using namespace Control;
        if (in.new_buttons & button_mask(Button::ACTION_1)) {
            Vec2 target = m_pos[1] + TOUCH_DIST * vec_from_direction(m_dir);
            float best = TOUCH_RADIUS * TOUCH_RADIUS;
            const Person *obj = nullptr;
            for (const auto &p : game.person()) {
                if (&p == this) {
                    continue;
                }
                Vec2 delta = p.m_pos[1] - target;
                float mag2 = delta.mag2();
                if (mag2 < best) {
                    best = mag2;
                    obj = &p;
                }
            }
            if (obj != nullptr) {
                game.machine().trigger_script(obj->m_identity);
            }
        }
    } else {
        in_move = Vec2::zero();
    }

    // Calculate velocity.
    Vec2 v0, v1;
    {
        v0 = m_vel;
        Vec2 vmove = in_move * MOVE_SPEED;
        Vec2 dv = vmove - v0;
        float dvmag = dv.mag();
        float faccel = dtime * ACCELERATION;
        v1 = dvmag <= faccel ? vmove : v0 + dv * (faccel / dvmag);
    }

    // Calculate physics push
    {
        auto tr = game.world().edge_distance(m_pos[1], m_is_player);
        float push_amt = PUSH_DIST - tr.first;
        if (push_amt > 0.0f) {
            float dc = Vec2::dot(tr.second, v1);
            if (dc > 0.0f)
                v1 += tr.second * (-dc);
        }
    }

    // Update position and input fields.
    m_pos[0] = m_pos[1];
    m_pos[1] = m_pos[1] + (v0 + v1) * (0.5 * dtime);
    m_posz[0] = m_posz[1];
    m_posz[1] = game.world().height_at(m_pos[1]) + HEIGHT * 0.5f;
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
                m_standtime = STAND_TIME;
                m_dir = direction_from_vec(step);
            } else {
                m_standtime -= dtime;
                if (m_standtime > 0.0f) {
                    walk = WALK_FRAME[m_stepframe];
                } else {
                    m_standtime = 0.0f;
                    walk = WALK_FRAME[WALK_STAND];
                }
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
                    sprite,
                    frames[static_cast<int>(group)],
                    0,
                    offsets[part]);
            }
            m_spritecount = pos;
        }
    }
}

}
