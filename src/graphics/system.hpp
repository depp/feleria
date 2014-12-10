/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GRAPHICS_SYSTEM_HPP
#define LD_GRAPHICS_SYSTEM_HPP
#include "shader.hpp"
#include "sprite.hpp"
#include "base/image.hpp"
#include "sg/opengl.h"
#include "sg/type.h"
#include <memory>
namespace Game {
class Game;
}
namespace Graphics {

/// The graphics system, responsible for drawing everything.
class System {
private:
    class SysUi;
    class SysText;
    class SysWorld;
    class SysSprite;

    std::unique_ptr<SysUi> m_ui;
    std::unique_ptr<SysText> m_text;
    std::unique_ptr<SysWorld> m_world;
    std::unique_ptr<SysSprite> m_sprite;

public:
    System();
    System(const System &) = delete;
    ~System();
    System &operator=(const System &) = delete;

    /// Load all graphical assets.
    bool load(const Game::Game &game);
    /// Draw the game's graphics.
    void draw(int width, int height, const Game::Game &game);
};

}
#endif
