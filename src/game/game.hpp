/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_GAME_HPP
#define LD_GAME_GAME_HPP
#include "defs.hpp"
#include "control.hpp"
#include "world.hpp"
#include "sprite.hpp"
#include "script.hpp"
#include "machine.hpp"
#include <vector>
namespace Game {
class Person;

class Game {
private:
    double m_dt;
    double m_frametime, m_curtime;
    float m_dtime;
    Control::Input m_input;
    Control::Bindings m_bindings;
    Control::FrameInput m_frame_input;

    Script m_script;
    Machine m_machine;
    SpriteData m_sprites;
    World m_world;
    std::vector<Person> m_person;

public:
    // ============================================================
    // Entry points
    // ============================================================

    Game();
    Game(const Game &) = delete;
    ~Game();
    Game &operator=(const Game &) = delete;

    /// Load all resources.
    bool load();

    /// Start a level.
    bool start_level(const std::string &name);

    /// Handle input from the user.
    void handle_event(const sg_event &evt);

    /// Update the world.
    void update(double time);

    // ============================================================
    // Modifying the game
    // ============================================================

    /// Add a person to the world.
    void add_person(const Person &person);

    // ============================================================
    // Game queries
    // ============================================================

    /// Get the absolute time for the current frame.
    double frame_abstime() const {
        return m_frametime;
    }

    /// Get the delta time for the current frame.  Used for updating
    /// the physics simulation.
    float frame_delta() const {
        return m_dtime;
    };

    /// Get the fraction of a frame elapsed since the last update.
    /// Used for interpolating between frames.
    float frame_frac() const {
        return (float) ((m_curtime - m_frametime) / m_dt);
    }

    /// Get the sprite data.
    const SpriteData &sprites() const {
        return m_sprites;
    }

    /// Get the game world.
    const World &world() const {
        return m_world;
    }

    /// Get all persons in the game.
    const std::vector<Person> &person() const {
        return m_person;
    }

    std::vector<Person> &person() {
        return m_person;
    }

private:
    void advance();
};

}
#endif
