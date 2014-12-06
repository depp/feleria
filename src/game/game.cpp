/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "game.hpp"
#include "control.hpp"
namespace Game {

namespace {
const double DEFAULT_DT = 1.0 / 30.0;
const double MAX_UPDATE = 1.0;
}

Game::Game()
    : m_frametime(0.0), m_curtime(0.0), m_dt(DEFAULT_DT) { }

Game::~Game() {}

void Game::handle_event(const sg_event &evt) {
    m_bindings.handle_event(m_input, evt);
}

void Game::update(double time) {
    double delta = time - m_curtime;
    if (m_curtime <= 0.0) {
        m_curtime = time;
        m_frametime = time;
    } else if (delta > MAX_UPDATE) {
        Log::warn("lag");
        double skip = delta - MAX_UPDATE;
        m_frametime += skip;
        m_curtime += skip;
    }

    double dtime = m_dt, start_time = m_frametime;
    while (true) {
        double end_time = start_time + dtime;
        if (end_time > time)
            break;
        auto input = m_input.read(start_time, end_time, true);
        m_move.update((float) dtime, input);
        m_frametime = start_time = end_time;
    }

    m_curtime = time;
}

}
