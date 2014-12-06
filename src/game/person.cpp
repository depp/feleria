/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "person.hpp"

namespace Game {

namespace {
const float MOVE_SPEED   = 5.0f;    // unit/s
const float ACCELERATION = 10.0f;   // unit/s^2
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

    // Update fields.
    m_in_flags = 0;
    m_in_move = Vec2::zero();
    m_pos[0] = m_pos[1];
    m_pos[1] = m_pos[1] + (v0 + v1) * (0.5 * dtime);
    m_vel = v1;
}

}
