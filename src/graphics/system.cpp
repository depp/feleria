/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "defs.hpp"
#include "system.hpp"
#include "game/game.hpp"
#include "game/person.hpp"
#include "sg/sprite.h"
namespace Graphics {

System::System() { }

System::~System() { }

void System::load() {
    m_prog_sprite.load("sprite", "sprite");
    m_sprite.load();
}

void System::draw(int width, int height, const Game::Game &game) {
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.1f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float frac = game.frame_frac();
    SpriteArray arr;
    const auto &sp = m_sprite.get(Game::Sprite::GOBLIN_BOT, 2, 0);
    for (const auto &p : game.person()) {
        Vec2 pos = Vec2::lerp(p.m_pos[0], p.m_pos[1], frac);
        arr.add(sp, (int) std::floor(pos[0]), (int) std::floor(pos[1]));
    }
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    arr.upload(GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    float vertxform[4] = {
        8.0f / (float) width,
        8.0f / (float) height,
        -1.0f,
        -1.0f
    };
    float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const auto &tex = m_sprite.texture();
    const auto &prog = m_prog_sprite;
    if (!prog.is_loaded())
        return;
    glUseProgram(prog.prog());

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glEnableVertexAttribArray(prog->a_vert);
    glVertexAttribPointer(prog->a_vert, 4, GL_SHORT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUniform4fv(prog->u_vertxform, 1, vertxform);
    glUniform2fv(prog->u_texscale, 1, tex.scale);
    glUniform1i(prog->u_texture, 0);
    glUniform4fv(prog->u_color, 1, color);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.tex);

    glDrawArrays(GL_TRIANGLES, 0, arr.size());

    glUseProgram(0);
    glDisable(GL_BLEND);

    sg_opengl_checkerror("System::draw");
}

}
