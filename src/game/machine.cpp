/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "machine.hpp"
#include "script.hpp"
#include "game.hpp"
namespace Game {

Machine::Machine(const Script &script)
    : m_script(script) {
    reset();
}

void Machine::reset() {
    m_pc = -1;
    m_character = -1;
}

bool Machine::jump(const std::string &name) {
    int target = m_script.get_label(name);
    if (target < 0) {
        return false;
    }
    m_pc = target;
    return true;
}

void Machine::run(Game &game) {
    (void) &game;
}

}
