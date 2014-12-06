/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_MOVE_HPP
#define LD_GAME_MOVE_HPP
#include "defs.hpp"
namespace Game {
namespace Control {
struct FrameInput;
}

class Move {
private:
    struct State {
        Vec2 pos;
        Vec2 vel;
    };

    State m_state[2];

public:
    Move();
    void update(float dtime, const Control::FrameInput &input);

    Vec2 position() const { return m_state[1].pos; }
};

}
#endif
