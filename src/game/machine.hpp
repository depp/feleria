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
    std::vector<int> m_memory;
    std::vector<std::pair<std::string, int>> m_options;

public:
    // ============================================================
    // Entry points
    // ============================================================

    explicit Machine(const Script &script);

    void reset();

    // Jump to the given label.
    bool jump(const std::string &name);

    // Run the machine for one frame.
    void run(Game &game);

    // Trigger a character's script.
    void trigger_script(int character);

    // ============================================================
    // Queries
    // ============================================================

private:
    void set_var(int var, int value);

    int get_var(int var) const;
};

}
#endif
