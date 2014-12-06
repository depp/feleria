/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_SYSTEM_HPP
#define LD_GRAPHICS_SYSTEM_HPP
#include "shader.hpp"
#include "sprite.hpp"
#include "sg/opengl.h"
namespace Game {
class Game;
}
namespace Graphics {

/// The graphics system, responsible for drawing everything.
class System {
private:
    Base::Program<Shader::Sprite> m_prog_sprite;
    SpriteSheet m_sprite;

public:
    System();
    System(const System &) = delete;
    ~System();
    System &operator=(const System &) = delete;

    /// Load all graphical assets.
    void load();
    /// Draw the game's graphics.
    void draw(int width, int height, const Game::Game &game);
};

}
#endif
