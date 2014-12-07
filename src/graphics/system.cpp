/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "defs.hpp"
#include "system.hpp"
#include "transform.hpp"
#include "color.hpp"
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

const int LIGHT_COUNT = 4;
const float LIGHT_DIR[LIGHT_COUNT][3] = {
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, 1.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    {-1.0f, 0.0f, 0.0f }
};
const float LIGHT_COLOR[LIGHT_COUNT][3] = {
    { 0.5f, 0.5f, 0.5f },
    { 1.0f, 0.5f, 0.5f },
    { 0.5f, 1.0f, 0.5f },
    { 0.5f, 0.5f, 1.0f }
};

const float SPRITE_SCALE = 0.2f;

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
        glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        s.count = size / 8;
        s.size = w.size();
    }
}

void System::draw(int width, int height, const Game::Game &game) {
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.1f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate perspective.
    Mat4 projection;
    Mat4 worldview;
    Quat camera_angle;
    {
        // Reference aspect ratio.
        const double ref_aspect = 16.0 / 9.0, inv_ref_aspect = 9.0 / 16.0;
        // 35mm equivalent focal length.
        const double focal = 55.0;
        // Width of the subject.
        const double subject_size = 64.0 * 1.4;

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
            camera_angle =
                Quat::rotation(
                    Vec3{{0.0f, 0.0f, 1.0f}},
                    (std::atan(1.0) / 45.0) * (180.0 - azimuth)) *
                Quat::rotation(
                    Vec3{{1.0f, 0.0f, 0.0f}},
                    (std::atan(1.0) / 45.0) * (90.0 - elevation));
            Vec3 target {{ 0.0f, 0.0f, 2.0f }};
            Vec3 dir = camera_angle.transform(Vec3{{0.0f, 0.0f, 1.0f}});
            Vec3 pos = target + dir * (float) distance;
            worldview = Mat4::rotation(camera_angle.conjugate()) *
                Mat4::translation(-pos);
        }
    }

    // Emit game geometry
    {
        float frac = game.frame_frac();
        Vec3 right = camera_angle.transform(Vec3{{SPRITE_SCALE, 0.0f, 0.0f}});
        Vec3 up = camera_angle.transform(Vec3{{0.0f, SPRITE_SCALE, 0.0f}});

        auto &s = m_sprite;
        s.array.clear();
        for (const auto &person : game.person()) {
            auto dir = DIRECTION_INFO[static_cast<int>(person.direction())];
            SpritePart parts[Game::PART_COUNT], *op = parts;
            auto partp = std::begin(person), parte = std::end(person);
            for (; partp != parte; partp++) {
                auto part = *partp;
                *op++ = SpritePart {
                    &s.sheet.get(part.sprite(), part.frame(), dir.index),
                    part.offset()
                };
            }
            s.array.add(parts, op - parts,
                        person.position(frac), right, up, dir.orient);
        }

        glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
        s.array.upload(GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        sg_opengl_checkerror("System::draw upload sprite");
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
                prog->a_vert, 4, GL_INT_2_10_10_10_REV, GL_FALSE,
                8, nullptr);
        }
        if (prog->a_normal >= 0) {
            glEnableVertexAttribArray(prog->a_normal);
            glVertexAttribPointer(
                prog->a_normal, 4, GL_INT_2_10_10_10_REV, GL_TRUE,
                8, reinterpret_cast<void *>(4));
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        float wscale = 64.0f / 512.0f;
        float wvscale = 0.5f;
        Transform xform = Transform::scale(Vec3{{
                    wscale, wscale, wvscale * wscale }});
        Color terrain_color[8] = {
            Color::palette(1),  Color::palette(2),
            Color::palette(10), Color::palette(20),
            Color::palette(3),  Color::palette(3),
            Color::palette(27),  Color::palette(27)
        };
        float height[8] = {
            -5.0f, +8.0f, 2.5f, 7.0f,
            -100.0f, -100.0f, -100.0f, -100.0f
        };
        float izscale = 1.0f / (wvscale * wscale);
        for (int i = 0; i < 8; i++)
            terrain_color[i].v[3] = izscale * height[i];

        xform.view = worldview * xform.view;
        glUniformMatrix4fv(prog->u_modelview, 1, GL_FALSE,
                           xform.view.data());
        glUniformMatrix4fv(prog->u_projection, 1, GL_FALSE,
                           projection.data());
        glUniformMatrix3fv(prog->u_normalmat, 1, GL_FALSE,
                           xform.normal.data());
        glUniform4fv(prog->u_terrain_color, 8, &terrain_color[0].v[0]);
        glUniform3fv(prog->u_light_dir, LIGHT_COUNT, LIGHT_DIR[0]);
        glUniform3fv(prog->u_light_color, LIGHT_COUNT, LIGHT_COLOR[0]);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, s.count);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        sg_opengl_checkerror("System::draw world");
    }

    // Draw sprites.
    if (m_sprite.prog.is_loaded()) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        auto &s = m_sprite;
        const auto &tex = s.sheet.texture();
        const auto &prog = s.prog;
        glUseProgram(prog.prog());

        glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
        if (prog->a_vert >= 0) {
            glEnableVertexAttribArray(prog->a_vert);
            glVertexAttribPointer(
                prog->a_vert, 3, GL_FLOAT, GL_FALSE,
                16, nullptr);
        }
        if (prog->a_texcoord >= 0) {
            glEnableVertexAttribArray(prog->a_texcoord);
            glVertexAttribPointer(
                prog->a_texcoord, 4, GL_SHORT, GL_FALSE,
                16, reinterpret_cast<void *>(12));
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUniformMatrix4fv(prog->u_modelview, 1, GL_FALSE,
                           worldview.data());
        glUniformMatrix4fv(prog->u_projection, 1, GL_FALSE,
                           projection.data());
        glUniform2fv(prog->u_texscale, 1, tex.scale);
        glUniform1i(prog->u_texture, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex.tex);

        glDrawArrays(GL_TRIANGLES, 0, s.array.size());

        sg_opengl_checkerror("System::draw sprite");
    }

    glUseProgram(0);
    glDisable(GL_BLEND);
}

}
