/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "game.hpp"
#include "control.hpp"
#include "person.hpp"
namespace Game {

namespace {
const double DEFAULT_DT = 1.0 / 30.0;
const double MAX_UPDATE = 1.0;
}

Game::Game()
    : m_frametime(0.0), m_curtime(0.0), m_dt(DEFAULT_DT) {
    m_person.push_back(Person(Vec2{{100.0f, 100.0f}}, Direction::LEFT));
    auto &p = m_person.back();
    p.set_part(Part::BOTTOM, Sprite::GOBLIN_BOT);
    p.set_part(Part::TOP, Sprite::GOBLIN_TOP);
    p.set_part(Part::HEAD, Sprite::GOBLIN_HEAD);
    m_person.push_back(Person(Vec2{{116.0f, 108.0f}}, Direction::RIGHT));
}

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
        m_person[0].m_in_flags = 0;
        m_person[0].m_in_move = input.move;
        m_frametime = start_time = end_time;

        for (auto &p : m_person) {
            p.update((float) dtime);
        }
    }

    m_curtime = time;
}

}
