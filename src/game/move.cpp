/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "move.hpp"
#include "control.hpp"

using Game::Control::Button;

namespace Game {

Move::Move() {
    m_state[0] = State { Vec2::zero(), Vec2::zero() };
    m_state[1] = m_state[0];
}

void Move::update(float dtime, const Control::FrameInput &input) {
    State state = m_state[1];
    m_state[0] = state;
}

}
