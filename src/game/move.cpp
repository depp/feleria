/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "move.hpp"
#include "control.hpp"

using Game::Control::Button;

namespace Game {

namespace {
const float MOVE_SPEED   = 5.0f;    // unit/s
const float ACCELERATION = 10.0f;   // unit/s^2
}

Move::Move() {
    m_state[0] = State { Vec2::zero(), Vec2::zero() };
    m_state[1] = m_state[0];
}

void Move::update(float dtime, const Control::FrameInput &input) {
    State state = m_state[1];
    m_state[0] = state;

    // Update velocity
    Vec2 v0, v1;
    {
        v0 = state.vel;
        Vec2 vmove = input.move * MOVE_SPEED;
        Vec2 dv = vmove - v0;
        float dvmag = dv.mag();
        float faccel = dtime * ACCELERATION;
        v1 = dvmag <= faccel ? vmove : v0 + dv * (faccel / dvmag);
    }
    state.vel = v1;

    // Update position
    state.pos += (v0 + v1) * (0.5f * dtime);

    m_state[1] = state;
}

}
