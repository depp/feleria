/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "person.hpp"

namespace Game {

namespace {
const float MOVE_SPEED      = 50.0f;    // unit/s
const float ACCELERATION    = 200.0f;   // unit/s^2
const float STEP_DISTANCE   = 4.0f;     // unit

const int WALK_TORSO[4] = { 3, 2, 3, 4 };
const int WALK_LEGS[4] = { 1, 2, 3, 4 };
}

Person::Person(Vec2 pos, Direction dir) {
    m_in_flags = 0;
    m_in_move = Vec2::zero();
    m_dir = dir;
    for (int i = 0; i < PART_COUNT; i++)
        m_part[i] = -1;
    for (int i = 0; i < GROUP_COUNT; i++)
        m_frame[i] = 0;
    m_pos[0] = m_pos[1] = pos;
    m_vel = Vec2::zero();
    m_steppos = pos;
    m_stepframe = 0;
}

void Person::update(float dtime) {
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

    // Update walking animation.
    {
        Vec2 step = m_pos[1] - m_steppos;
        float stepd2 = step.mag2();
        if (stepd2 >= STEP_DISTANCE * STEP_DISTANCE) {
            float stepd = std::sqrt(stepd2);
            float nstep = stepd / STEP_DISTANCE;
            m_steppos += step * (std::floor(nstep) * STEP_DISTANCE / stepd);
            m_stepframe = (m_stepframe + (int) nstep) & 3;
            set_frame(Group::LEGS, WALK_LEGS[m_stepframe]);
            set_frame(Group::TORSO, WALK_TORSO[m_stepframe]);
        }
    }
}

}
