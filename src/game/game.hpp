/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_GAME_HPP
#define LD_GAME_GAME_HPP
#include "defs.hpp"
#include "move.hpp"
#include "control.hpp"
namespace Game {

class Game {
public:
    double m_frametime, m_curtime, m_dt;
    Move m_move;
    Control::Input m_input;
    Control::Bindings m_bindings;

    Game();
    Game(const Game &) = delete;
    ~Game();
    Game &operator=(const Game &) = delete;

    void handle_event(const sg_event &evt);
    void update(double time);
};

}
#endif
