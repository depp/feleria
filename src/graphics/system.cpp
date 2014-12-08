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
#include "base/image.hpp"
#include <cstring>
namespace Graphics {

namespace {

const bool debug_trace = false;
const float FONT_SIZE = 48.0f;
const float BOX_SIZE = 1024.0f;
const Vec2 TEXT_POS {{ -425.0f, 325.0f }};

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

System::SysText::SysText()
    : typeface(nullptr),
      font(nullptr),
      empty(true),
      serial(0xffffffff),
      buffer(0){
    for (int i = 0; i < LINE_COUNT; i++) {
        line[i].layout = nullptr;
    }
}

System::System() { }

System::~System() { }

bool System::load(const Game::Game &game) {
    bool success = true;
    {
        auto &s = m_sprite;
        s.prog.load("sprite", "sprite");
        if (!s.texture.load("image/sprite")) {
            success = false;
        } else {
            glBindTexture(GL_TEXTURE_2D, s.texture.tex);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glDeleteBuffers(1, &s.buffer);
        glGenBuffers(1, &s.buffer);
        s.util_sprite = game.sprites().get_index("ui_util");
    }
    {
        auto &s = m_world;
        if (!s.prog.load("world", "world")) {
            success = false;
        } else {
            glDeleteBuffers(1, &s.buffer);
            glGenBuffers(1, &s.buffer);
            const auto &w = game.world();
            auto vdata = w.vertex_data();
            glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
            glBufferData(GL_ARRAY_BUFFER,
                         vdata.second, vdata.first, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            s.count = vdata.second / 8;
        }
    }
    {
        auto &s = m_text;
        if (!s.prog.load("text", "text")) {
            success = false;
        }
        if (!s.uiprog.load("ui", "ui")) {
            success = false;
        }
        const char *path = "font/Alegreya-Bold";
        auto typeface = sg_typeface_file(path, std::strlen(path), nullptr);
        if (typeface == nullptr) {
            success = false;
        } else {
            if (s.typeface) {
                sg_typeface_decref(s.typeface);
            }
            s.typeface = typeface;
        }
        if (!s.buffer) {
            glGenBuffers(1, &s.buffer);
        }
        if (!s.textbox.load("image/textbox")) {
            success = false;
        } else {
            glBindTexture(GL_TEXTURE_2D, s.textbox.tex);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    return success;
}

void System::draw(int width, int height, const Game::Game &game) {
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.1f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate perspective.
    Mat4 projection;
    Mat4 worldview;
    Quat camera_angle;
    float pixscale;
    // Vec2 inner_size;
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
                pixscale = (float) height * (1.0f / 1080.0f);
            } else {
                yratio = xratio / aspect;
                pixscale = (float) width * (1.0f / 1920.0f);
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
        const auto &sd = game.sprites();
        s.array.clear();
        for (const auto &person : game.person()) {
            auto dir = DIRECTION_INFO[static_cast<int>(person.direction())];
            SpritePart parts[Game::PART_COUNT], *op = parts;
            for (auto part : person.sprite()) {
                *op++ = SpritePart {
                    &sd.get_data(part.sprite(), part.frame(), dir.index),
                    part.offset()
                };
            }
            s.array.add(parts, op - parts,
                        person.position(frac), right, up, dir.orient);

            if (debug_trace) {
                const auto &w = game.world();
                auto pos = person.position(frac);
                Vec2 pos2 {{ pos[0], pos[1] }};
                auto trace = w.edge_distance(pos2, true);
                int frame = trace.first > 0.0f ? 0 : 1;
                parts[0] = SpritePart {
                    &sd.get_data(s.util_sprite, frame, 0),
                    Vec2::zero()
                };
                s.array.add(
                    parts, 1,
                    w.project(pos2 + trace.first * trace.second),
                    right, up, Orientation::NORMAL);
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
        s.array.upload(GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        sg_opengl_checkerror("System::draw upload sprite");
    }

    const int NLINE = SysText::LINE_COUNT;

    // Compute text layout.
    if (m_text.serial != game.machine().text_serial()) {
        const auto &vm = game.machine();
        auto &s = m_text;
        const auto &text = vm.text();
        s.serial = vm.text_serial();

        for (int i = 0; i < NLINE; i++) {
            if (s.line[i].layout) {
                sg_textlayout_free(s.line[i].layout);
            }
            s.line[i].layout = nullptr;
        }
        s.empty = true;

        if (!text.empty()) {
            bool load_font = !s.font;
            if (load_font) {
                Log::debug("PIXSCALE: %f", pixscale);
                auto font = sg_font_new(
                    s.typeface, FONT_SIZE * pixscale, nullptr);
                if (s.font) {
                    sg_font_decref(s.font);
                }
                s.font = font;
            }
        }

        Vec2 vertscale { 2.0f / width, 2.0f / height };
        Vec2 pos = TEXT_POS * pixscale;
        int n = (int) std::min((std::size_t) NLINE, text.size());
        if (n) n = 4;
        for (int i = 0; i < n; i++) {
            const char *ltext = text[0].text;
            auto flow = sg_textflow_new(nullptr);
            if (!flow) {
                break;
            }
            sg_textflow_setfont(flow, s.font);
            sg_textflow_addtext(flow, ltext, std::strlen(ltext));

            auto layout = sg_textlayout_new(flow, nullptr);
            sg_textflow_free(flow);
            if (!layout) {
                break;
            }
            s.line[i].layout = layout;
            sg_textmetrics metrics;
            sg_textlayout_getmetrics(layout, &metrics);
            Vec2 lpos = pos;
            lpos[0] -= (float) metrics.logical.x0;
            lpos[1] -= (float) metrics.logical.y1;
            {
                float *v = s.line[i].vertxform;
                v[0] = vertscale[0];
                v[1] = vertscale[1];
                v[2] = lpos[0] * vertscale[0];
                v[3] = lpos[1] * vertscale[1] - 1.0f;
            }
            s.empty = false;
            pos[1] -= (float) (metrics.logical.y1 - metrics.logical.y0);
        }

        {
            float boxsize = BOX_SIZE * pixscale;
            float x0 = -0.5f * boxsize * vertscale[0];
            float x1 = +0.5f * boxsize * vertscale[0];
            float y0 = -1.0f;
            float y1 = -1.0f + vertscale[1] * boxsize * 0.5f;
            Log::debug("BOXSIZE: %f %f %f %f", x0, x1, y0, y1);
            float vert[24] = {
                x0, y0, 0.0f, 1.0f,
                x1, y0, 1.0f, 1.0f,
                x0, y1, 0.0f, 0.0f,
                x0, y1, 0.0f, 0.0f,
                x1, y0, 1.0f, 1.0f,
                x1, y1, 1.0f, 0.0f
            };
            glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert,
                         GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    // Draw text box.
    if (m_text.uiprog.is_loaded()) {
        const auto &s = m_text;
        const auto &prog = s.uiprog;

        glUseProgram(prog.prog());

        glBindBuffer(GL_ARRAY_BUFFER, s.buffer);
        if (prog->a_vert >= 0) {
            glEnableVertexAttribArray(prog->a_vert);
            glVertexAttribPointer(
                prog->a_vert, 4, GL_FLOAT, GL_FALSE,
                0, nullptr);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glUniform1i(prog->u_texture, 0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
        glDepthRange(0.0, 0.0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s.textbox.tex);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glUseProgram(0);
        glDepthFunc(GL_LESS);
        glDepthRange(0.0, 1.0);
    }

    // Draw text.
    if (m_text.prog.is_loaded() && !m_text.empty) {
        const auto &s = m_text;
        const auto &prog = s.prog;

        glUseProgram(prog.prog());

        // texscale handled by draw()
        glUniform1i(prog->u_texture, 0);
        Color color = Color::palette(21);
        glUniform4fv(prog->u_color, 1, color.v);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
        glDepthRange(0.0, 0.0);

        for (int i = 0; i < NLINE; i++) {
            const auto &line = s.line[i];
            if (!line.layout) {
                break;
            }
            glUniform4fv(prog->u_vertxform, 1, line.vertxform);
            sg_textlayout_draw(
                line.layout, prog->a_vert, prog->u_texscale);
        }

        glUseProgram(0);
        glDepthFunc(GL_LESS);
        glDepthRange(0.0, 1.0);
    }

    // Draw world.
    if (m_world.prog.is_loaded()) {
        const auto &s = m_world;
        const auto &w = game.world();
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

        auto scale = w.vertex_scale();
        Mat4 modelview = worldview * Mat4::scale(scale);
        Mat3 normalmat = Mat3::identity();
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
        for (int i = 0; i < 8; i++) {
            terrain_color[i].v[3] = height[i] * (1.0f / scale[2]);
        }

        glUniformMatrix4fv(prog->u_modelview, 1, GL_FALSE,
                           modelview.data());
        glUniformMatrix4fv(prog->u_projection, 1, GL_FALSE,
                           projection.data());
        glUniformMatrix3fv(prog->u_normalmat, 1, GL_FALSE,
                           normalmat.data());
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
        auto &s = m_sprite;
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
        glUniform2fv(prog->u_texscale, 1, s.texture.scale);
        glUniform1i(prog->u_texture, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s.texture.tex);

        glEnable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, s.array.size());
        glDisable(GL_DEPTH_TEST);

        sg_opengl_checkerror("System::draw sprite");
    }

    glUseProgram(0);
    glDisable(GL_BLEND);
}

}
