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
    : m_dt(DEFAULT_DT), m_frametime(0.0), m_curtime(0.0),
      m_dtime(0.0f) {
    Person p(Vec2::zero(), Direction::LEFT);
    p.set_part(Part::BOTTOM, Sprite::GIRL_BOT);
    p.set_part(Part::TOP, Sprite::GIRL_TOP);
    p.set_part(Part::HEAD, Sprite::GIRL_HEAD);
    p.set_part(Part::ITEM1, Sprite::ITEM_SWORD);
    add_person(p);
}

Game::~Game() {}

bool Game::load() {
    bool success = true;
    if (!m_world.load()) {
        Log::warn("Could not load world.");
        success = false;
    }
    return success;
}

void Game::handle_event(const sg_event &evt) {
    m_bindings.handle_event(m_input, evt);
}

void Game::update(double time) {
    double dtime = m_dt;
    m_dtime = (float) dtime;
    if (m_curtime <= 0.0) {
        m_curtime = m_frametime = time;
        m_frame_input = m_input.read(0.0, time, true);
        advance();
        return;
    }

    double delta = time - m_curtime;
    if (delta > MAX_UPDATE) {
        Log::warn("lag");
        m_frametime += delta - MAX_UPDATE;
    }

    m_curtime = time;

    double start_time = m_frametime;
    while (true) {
        double end_time = start_time + dtime;
        if (end_time > time)
            break;
        m_frametime = end_time;
        m_frame_input = m_input.read(start_time, end_time, true);
        advance();
        start_time = end_time;
    }
}

void Game::add_person(const Person &person) {
    m_person_pending.push_back(person);
}

void Game::advance() {
    if (!m_person.empty()) {
        m_person[0].set_input(m_frame_input.move, 0);
    }

    for (std::size_t pos = 0; pos < m_person_pending.size(); pos++) {
        m_person.push_back(m_person_pending[pos]);
        m_person.back().initialize(*this);
    }
    m_person_pending.clear();

    for (auto &p : m_person) {
        p.update(*this);
    }
}

}
