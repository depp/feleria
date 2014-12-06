/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_GAME_HPP
#define LD_GAME_GAME_HPP
#include "defs.hpp"
#include "control.hpp"
#include <vector>
namespace Game {
namespace Control {
struct FrameInput;
}
class Person;

class Game {
private:
    double m_frametime, m_curtime, m_dt;
    Control::Input m_input;
    Control::Bindings m_bindings;
    std::vector<Person> m_person;

public:
    Game();
    Game(const Game &) = delete;
    ~Game();
    Game &operator=(const Game &) = delete;

    /// Handle input from the user.
    void handle_event(const sg_event &evt);

    /// Update the world.
    void update(double time);

    /// Get the fraction of a frame.
    float frame_frac() const {
        return (float) ((m_curtime - m_frametime) / m_dt);
    }

    /// Get all persons in the game.
    const std::vector<Person> &person() const {
        return m_person;
    }

private:
    void advance(double abstime, float dtime,
                 const Control::FrameInput &input);

};

}
#endif
