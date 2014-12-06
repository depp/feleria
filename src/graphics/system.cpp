/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "defs.hpp"
#include "system.hpp"
#include "game/game.hpp"
#include "game/person.hpp"
#include "sg/sprite.h"

#include "sg/sprite.h"
namespace Graphics {

System::System() { }

System::~System() { }

void System::load() {
    auto &s = m_sprite;
    s.m_prog.load("sprite", "sprite");
    s.m_sheet.load();
    glDeleteBuffers(1, &s.m_buffer);
    glGenBuffers(1, &s.m_buffer);
}

void System::draw(int width, int height, const Game::Game &game) {
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.1f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Emit game geometry
    {
        float frac = game.frame_frac();

        auto &s = m_sprite;
        s.m_array.clear();
        static const int PART_GROUP[Game::PART_COUNT] = {
            2, 2, 1, 2, 0, 3, 0
        };
        for (const auto &p : game.person()) {
            Vec2 pos = Vec2::lerp(p.m_pos[0], p.m_pos[1], frac);
            for (int i = 0; i < Game::PART_COUNT; i++) {
                int spi = p.m_part[i];
                if (spi < 0)
                    continue;
                int fr = p.m_frame[PART_GROUP[i]];
                const auto &sp = s.m_sheet.get(
                    static_cast<Game::Sprite>(spi), fr, 0);
                s.m_array.add(sp, pos[0], pos[1], Base::Orientation::NORMAL);
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, s.m_buffer);
        s.m_array.upload(GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Draw sprites.
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        float vertxform[4] = {
            8.0f / (float) width,
            8.0f / (float) height,
            -1.0f,
            -1.0f
        };
        float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        {
            auto &s = m_sprite;
            const auto &tex = s.m_sheet.texture();
            const auto &prog = s.m_prog;
            if (!prog.is_loaded())
                return;
            glUseProgram(prog.prog());

            glBindBuffer(GL_ARRAY_BUFFER, s.m_buffer);
            glEnableVertexAttribArray(prog->a_vert);
            glEnableVertexAttribArray(prog->a_texcoord);
            glVertexAttribPointer(prog->a_vert, 2, GL_FLOAT, GL_FALSE,
                                  12, nullptr);
            glVertexAttribPointer(prog->a_texcoord, 4, GL_SHORT, GL_FALSE,
                                  12, reinterpret_cast<void *>(8));
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glUniform4fv(prog->u_vertxform, 1, vertxform);
            glUniform2fv(prog->u_texscale, 1, tex.scale);
            glUniform1i(prog->u_texture, 0);
            glUniform4fv(prog->u_color, 1, color);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex.tex);

            glDrawArrays(GL_TRIANGLES, 0, s.m_array.size());
        }

        glUseProgram(0);
        glDisable(GL_BLEND);
    }

    sg_opengl_checkerror("System::draw");
}

}
