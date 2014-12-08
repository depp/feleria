/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_MACHINE_HPP
#define LD_GAME_MACHINE_HPP
#include <vector>
#include <string>
namespace Game {
class Script;
class Game;

class Machine {
private:
    const Script &m_script;

    int m_pc;
    int m_character;

public:
    // ============================================================
    // Entry points
    // ============================================================

    explicit Machine(const Script &script);

    void reset();

    bool jump(const std::string &name);

    void run(Game &game);

    // ============================================================
    // Queries
    // ============================================================

};

}
#endif
