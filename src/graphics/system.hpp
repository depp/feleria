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
namespace Game {
class Game;
}
namespace Graphics {

/// The graphics system, responsible for drawing everything.
class System {
private:
    struct SysSprite {
        Base::Program<Shader::Sprite> prog;
        Base::Texture texture;
        SpriteArray array;
        GLuint buffer;
        int util_sprite;

        SysSprite() : buffer(0), util_sprite(-1) {}
    };

    struct SysWorld {
        Base::Program<Shader::World> prog;
        GLuint buffer;
        GLsizei count;

        SysWorld() : buffer(0), count(0) {}
    };

    struct SysText {
        static const int LINE_COUNT = 4;

        struct Line {
            sg_textlayout *layout;
            float vertxform[4];
        };

        Base::Program<Shader::Text> prog;
        Base::Program<Shader::Ui> uiprog;
        sg_typeface *typeface;
        sg_font *font;
        bool empty;
        Line line[LINE_COUNT];
        unsigned serial;
        GLuint buffer;
        Base::Texture textbox;

        SysText();
    };

    SysSprite m_sprite;
    SysWorld m_world;
    SysText m_text;

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
