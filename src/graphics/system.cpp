/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "defs.hpp"
#include "system.hpp"
#include "game/game.hpp"
#include "game/person.hpp"
namespace Graphics {

namespace {

using Base::Orientation;

struct DirectionInfo {
    int index;
    Orientation orient;
};

const DirectionInfo DIRECTION_INFO[4] = {
    { 1, Orientation::FLIP_HORIZONTAL },
    { 2, Orientation::NORMAL },
    { 1, Orientation::NORMAL },
    { 0, Orientation::NORMAL },
};

}

System::System() { }

System::~System() { }

void System::load(const Game::Game &game) {
    {
        auto &s = m_sprite;
        s.prog.load("sprite", "sprite");
        s.sheet.load();
        glDeleteBuffers(1, &s.buffer);
        glGenBuffers(1, &s.buffer);
    }
    {
        auto &s = m_world;
        s.prog.load("world", "world");
        glDeleteBuffers(1, &s.buffer);
        glGenBuffers(1, &s.buffer);
        const auto &w = game.world();
        auto data = w.vertex_data();
        auto size = w.vertex_size();
        const unsigned *p = reinterpret_cast<const unsigned *>(data);
        for (int i = 0; i < 10; i++) {
            Log::debug("Vertex: %08x", p[i*3]);
        }
        glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        s.count = size / 12;
        s.size = w.size();
    }
}

void System::draw(int width, int height, const Game::Game &game) {
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.1f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Emit game geometry
    {
        float frac = game.frame_frac();

        auto &s = m_sprite;
        s.array.clear();
        for (const auto &person : game.person()) {
            auto dir = DIRECTION_INFO[static_cast<int>(person.direction())];
            Vec2 pos = person.position(frac);
            auto partp = std::begin(person), parte = std::end(person);
            for (; partp != parte; partp++) {
                auto part = *partp;
                s.array.add(
                    s.sheet.get(part.sprite(), part.frame(), dir.index),
                    pos + part.offset(),
                    dir.orient);
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
        s.array.upload(GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        sg_opengl_checkerror("System::draw upload sprite");
    }

    // Calculate perspective.
    Mat4 projection, worldview;
    {
        // Reference aspect ratio.
        const double ref_aspect = 16.0 / 9.0, inv_ref_aspect = 9.0 / 16.0;
        // 35mm equivalent focal length.
        const double focal = 55.0;
        // Width of the subject.
        const double subject_size = 64.0;

        double distance;

        {
            // We pretend that we are using a 16:9 aspect ratio.
            double xratio = 18.0 / focal;
            double yratio = xratio * inv_ref_aspect;
            distance = 0.5 * subject_size / xratio;

            // Then we expand the FOV to match the actual aspect ratio.
            double aspect = (double) width / (double) height;
            if (aspect > ref_aspect) {
                xratio = yratio * aspect;
            } else {
                yratio = xratio / aspect;
            }

            projection = Mat4::perspective(
                (float) xratio,
                (float) yratio,
                std::max(1.0f, (float) (distance - 0.5 * subject_size)),
                (float) (distance + 0.5 * subject_size));
        }

        // View angle.
        const double azimuth = 180.0;
        const double elevation = 40.0;

        {
            Quat angle =
                Quat::rotation(
                    Vec3{{0.0f, 0.0f, 1.0f}},
                    (std::atan(1.0) / 45.0) * (180.0 - azimuth)) *
                Quat::rotation(
                    Vec3{{1.0f, 0.0f, 0.0f}},
                    (std::atan(1.0) / 45.0) * (90.0 - elevation));
            Vec3 target {{ 0.5f * (float) m_world.size[0],
                           0.5f * (float) m_world.size[1],
                           0.0f }};
            Vec3 dir = angle.transform(Vec3{{0.0f, 0.0f, 1.0f}});
            Vec3 pos = target + dir * (float) distance;
            worldview = Mat4::rotation(angle.conjugate()) *
                Mat4::translation(-pos);
        }
    }

    // Draw world.
    if (m_world.prog.is_loaded()) {
        const auto &s = m_world;
        const auto &prog = s.prog;

        glUseProgram(prog.prog());

        glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
        if (prog->a_vert >= 0) {
            glEnableVertexAttribArray(prog->a_vert);
            glVertexAttribPointer(
                prog->a_vert, 3, GL_UNSIGNED_BYTE, GL_FALSE,
                12, nullptr);
        }
        if (prog->a_color >= 0) {
            glEnableVertexAttribArray(prog->a_color);
            glVertexAttribPointer(
                prog->a_color, 3, GL_UNSIGNED_BYTE, GL_TRUE,
                12, reinterpret_cast<void *>(4));
        }
        if (prog->a_normal >= 0) {
            glEnableVertexAttribArray(prog->a_normal);
            glVertexAttribPointer(
                prog->a_normal, 3, GL_INT_2_10_10_10_REV, GL_FALSE,
                12, reinterpret_cast<void *>(8));
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUniformMatrix4fv(prog->u_modelview, 1, GL_FALSE,
                           worldview.data());
        glUniformMatrix4fv(prog->u_projection, 1, GL_FALSE,
                           projection.data());

        glDrawArrays(GL_TRIANGLES, 0, s.count);

        sg_opengl_checkerror("System::draw world");
    }

    // Draw sprites.
    if (m_sprite.prog.is_loaded()) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        float vertxform[4] = {
            8.0f / (float) width,
            8.0f / (float) height,
            -1.0f,
            -1.0f
        };
        float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        auto &s = m_sprite;
        const auto &tex = s.sheet.texture();
        const auto &prog = s.prog;
        glUseProgram(prog.prog());

        glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
        if (prog->a_vert >= 0) {
            glEnableVertexAttribArray(prog->a_vert);
            glVertexAttribPointer(
                prog->a_vert, 2, GL_FLOAT, GL_FALSE,
                12, nullptr);
        }
        if (prog->a_texcoord >= 0) {
            glEnableVertexAttribArray(prog->a_texcoord);
            glVertexAttribPointer(
                prog->a_texcoord, 4, GL_SHORT, GL_FALSE,
                12, reinterpret_cast<void *>(8));
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUniform4fv(prog->u_vertxform, 1, vertxform);
        glUniform2fv(prog->u_texscale, 1, tex.scale);
        glUniform1i(prog->u_texture, 0);
        glUniform4fv(prog->u_color, 1, color);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex.tex);

        glDrawArrays(GL_TRIANGLES, 0, s.array.size());

        sg_opengl_checkerror("System::draw sprite");
    }

    glUseProgram(0);
    glDisable(GL_BLEND);
}

}
